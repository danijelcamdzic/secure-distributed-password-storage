/**
 * @file mqtt_functions.c
 * @brief Contains helper mqtt functions implementation
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#include "main.h"
#include "mqtt_functions.h"
#include "nvs_functions.h"
#include "MQTT311Client/MQTT311Client.h"
#include "RSA/RSA.h"

/**
 * @brief Find the index of a substring within the MQTT311_RECEIVE_BUFFER.
 *
 * This function searches for the first occurrence of the specified substring
 * within the MQTT311_RECEIVE_BUFFER and returns the index at which the substring
 * is found. If the substring is not found, it returns -1. If the specified
 * substring length is larger than the MQTT311_RECEIVED_BYTES, it also returns -1.
 *
 * @param substr Pointer to the null-terminated substring to search for.
 * @param substr_len Length of the substring to search for.
 * @return The index of the first occurrence of the substring within the
 *         MQTT311_RECEIVE_BUFFER, or -1 if not found or if substr_len is larger
 *         than MQTT311_RECEIVED_BYTES.
 */
int mqtt_find_substring_index(const char *substr, size_t substr_len) {
    if (substr_len > MQTT311_RECEIVED_BYTES) {
        return -1;
    }

    for (size_t i = 0; i <= MQTT311_RECEIVED_BYTES - substr_len; ++i) {
        bool match = true;
        for (size_t j = 0; j < substr_len; ++j) {
            if (MQTT311_RECEIVE_BUFFER[i + j] != substr[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Receive a passphrase from the MQTT311_RECEIVE_BUFFER and copy it to the RSA_ENCRYPTED_BUFFER.
 *
 * This function reads a passphrase from the MQTT311_RECEIVE_BUFFER starting from
 * index_start and ending at index_end (exclusive). The passphrase is then copied
 * to the RSA_ENCRYPTED_BUFFER. The function also logs the number of bytes received,
 * and starts the decryption task using the RSA decryption function.
 *
 * @param index_start The starting index of the passphrase in the MQTT311_RECEIVE_BUFFER.
 * @param index_end The ending index (exclusive) of the passphrase in the MQTT311_RECEIVE_BUFFER.
 */
void mqtt_receive_passphrase(int index_start, int index_end)
{
    char* TAG = "mqtt_receive_passphrase"; // Declare and initialize TAG for logging purposes 

    for (int i = index_start; i < index_end; i++)
    {
        printf("%c ", MQTT311_RECEIVE_BUFFER[i]);
        RSA_ENCRYPTED_BUFFER[i - index_start] = MQTT311_RECEIVE_BUFFER[i];
    }
    printf("\n");

    /* Check the received data length */
    RSA_MESSAGE_LENGTH = index_end - index_start;
    ESP_LOGI(TAG, "Received: %d bytes", RSA_MESSAGE_LENGTH);

    /* Decrypt the data received with hw private key */
    ESP_LOGI(TAG, "Begin decryption of received data");
    RSA_StartDecryptionTask();
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Finished decryption");

    /* Check length after decryption */
    ESP_LOGI(TAG, "Length after decrypting %d bytes", RSA_MESSAGE_LENGTH);

    /* Begin encryption of the data with master publci key */
    ESP_LOGI(TAG, "Begin encryption of received data");
    free(RSA_MESSAGE_TO_ENCRYPT);
    RSA_MESSAGE_TO_ENCRYPT = malloc(RSA_MESSAGE_LENGTH);
    memcpy(RSA_MESSAGE_TO_ENCRYPT, RSA_ENCRYPTED_BUFFER, RSA_MESSAGE_LENGTH);
    RSA_StartEncryptionTask();
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Finished encryption");

    /* Check the encryption data length */
    ESP_LOGI(TAG, "Data length after encryption: %d bytes", RSA_MESSAGE_LENGTH);

    /* Store the data into NVS */
    ESP_LOGI(TAG, "Begin storing data in NVS");
    const char *pass_key = PASSWORD_STORE_KEY;
    char* password = malloc(RSA_MESSAGE_LENGTH);
    memcpy(password, RSA_ENCRYPTED_BUFFER, RSA_MESSAGE_LENGTH);
    nvs_store(pass_key, password, RSA_MESSAGE_LENGTH);
    ESP_LOGI(TAG, "Finished storing data in NVS");
    vTaskDelay(pdMS_TO_TICKS(3000));
}

/**
 * @brief Send passphrase to MQTT311 topic
 *
 * This function reads the encrypted value from NVMem and sends it to MQTT311 topic.
 *
 * @param None
 */
void mqtt_send_passphrase()
{
    char* TAG = "mqtt_send_passphrase";                                  // Declare and initialize TAG for logging purposes  

    /* Read the value from NVS */
    const char *pass_key = PASSWORD_STORE_KEY;
    char* read_value = nvs_read(pass_key);

    /* Publish the read value */
    MQTT311Client_Publish(0x00, PUB_TOPIC, 0x00, read_value, 256);
    vTaskDelay(pdMS_TO_TICKS(3000));

    free(read_value);                                           // Free the allocated memory
    ESP_LOGI(TAG, "Finished publishing the passphrase");
}

/**
 * @brief Search for commands within the MQTT311_RECEIVE_BUFFER
 *
 * This function searches for commands within the MQTT311_RECEIVE_BUFFER and 
 * does appropriate actions according to it. It either receives the passphrase and stores it in NVmem
 * or sends it from NVmem to some MQTT311 topic
 *
 * @param None
 */
void mqtt_process_buffer_data(void) 
{
    char* TAG = "mqtt_process_buffer_data";                          // Declare and initialize TAG for logging purposes  

    char* str1 = ALL_TOPIC;
    char* str2 = END_MESSAGE_FLAG;

    int index_start = mqtt_find_substring_index(str1, strlen(str1));
    int index_end = mqtt_find_substring_index(str2, strlen(str2));

    if ((index_start != -1) && (index_end != -1)) {
        ESP_LOGI(TAG, "Received command to send password...");
        mqtt_send_passphrase();

        /* Reset the buffer and return */
        MQTT311_RECEIVED_BYTES = 0;
        return;
    }

    char* str3 = SUB_TOPIC;
    char* str4 = END_MESSAGE_FLAG;

    index_start = mqtt_find_substring_index(str3, strlen(str3));
    index_end = mqtt_find_substring_index(str4, strlen(str4));

    if ((index_start != -1) && (index_end != -1)) {
        ESP_LOGI(TAG, "Received command to store password...");
        mqtt_receive_passphrase(index_start + strlen(str3), index_end);
    }

    /* Reset the buffer */
    MQTT311_RECEIVED_BYTES = 0;
}

