/**
 * @file mqtt_functions.c
 * @brief Contains helper mqtt functions implementation
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#include "main.h"
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
int find_substring_index(const char *substr, size_t substr_len) {
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
void receive_passphrase(int index_start, int index_end)
{
    char* TAG = "receive_passphrase"; // Declare and initialize TAG for logging purposes  

    int i = 0;

    for (i = index_start ; i < index_end; i++)
    {
        printf("%c ", MQTT311_RECEIVE_BUFFER[i]);
        RSA_ENCRYPTED_BUFFER[i - index_start] = MQTT311_RECEIVE_BUFFER[i];
    }
    printf("\n");

    RSA_MESSAGE_LENGTH = index_end - index_start;

    ESP_LOGI(TAG, "Received: %d bytes", RSA_MESSAGE_LENGTH);
    ESP_LOGI(TAG, "Begin encryption and/or decryption");

    // Encrypt and decrypt text
    // RSA_SetPublicKeyInUse((PublicKeyControl_t)MASTER_PUBLIC_KEY)
    // RSA_StartEncryptionTask();
    // vTaskDelay(pdMS_TO_TICKS(3000));
    RSA_StartDecryptionTask();
}

/**
 * @brief Send passphrase to MQTT311 topic
 *
 * This function reads the encrypted value from NVMem and sends it to MQTT311 topic.
 *
 * @param None
 */
void send_passphrase()
{
    // RSA_MESSAGE_TO_ENCRYPT = (char*) pvPortMalloc(index2-index1-strlen(str1));
    // memcpy(RSA_MESSAGE_TO_ENCRYPT, (void*)&MQTT311_RECEIVE_BUFFER[index1 + strlen(str1)], index2-index1-strlen(str1));

    // // Null-terminate the copied string
    // RSA_MESSAGE_TO_ENCRYPT[index2-index1-strlen(str1)] = '\0';
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
void process_buffer_data(void) 
{
    char* str1 = "/topic/topic3";
    char* str2 = "END_MESSAGE";

    int index_start = find_substring_index(str1, strlen(str1));
    int index_end = find_substring_index(str2, strlen(str2));

    if ((index_start != -1) && (index_end != -1)) {
        receive_passphrase(index_start + strlen(str1), index_end);
    }

    MQTT311_RECEIVED_BYTES = 0;
}

