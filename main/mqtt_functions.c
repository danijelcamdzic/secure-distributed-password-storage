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

void send_passphrase()
{
    // RSA_MESSAGE_TO_ENCRYPT = (char*) pvPortMalloc(index2-index1-strlen(str1));
    // memcpy(RSA_MESSAGE_TO_ENCRYPT, (void*)&MQTT311_RECEIVE_BUFFER[index1 + strlen(str1)], index2-index1-strlen(str1));

    // // Null-terminate the copied string
    // RSA_MESSAGE_TO_ENCRYPT[index2-index1-strlen(str1)] = '\0';
}

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

