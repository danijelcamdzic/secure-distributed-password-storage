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
    if (substr_len > number_of_bytes_received) {
        return -1;
    }

    for (size_t i = 0; i <= number_of_bytes_received - substr_len; ++i) {
        bool match = true;
        for (size_t j = 0; j < substr_len; ++j) {
            if (bytes_to_receive[i + j] != substr[j]) {
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

void process_buffer_data(void) 
{
    char* TAG = "process_buffer_data"; // Declare and initialize TAG for logging purposes  

    ESP_LOGI(TAG, "Here is the total length of the buffer so far: %d ", number_of_bytes_received);

    char* str1 = "/topic/topic3";
    char* str2 = "END_MESSAGE";

    int index1 = find_substring_index(str1, strlen(str1));
    int index2 = find_substring_index(str2, strlen(str2));

    if ((index1 != -1) && (index2 != -1)) {
        int i = 0;

        for (i = index1 + strlen(str1); i < index2; i++)
        {
            printf("%c ", bytes_to_receive[i]);
            buf[i - index1 - strlen(str1)] = bytes_to_receive[i];
        }
        printf("\n");

        length = index2-index1-strlen(str1);

        printf("Received: %d bytes", length);

        // text_to_encrypt = (char*) pvPortMalloc(index2-index1-strlen(str1));
        // memcpy(text_to_encrypt, (void*)&bytes_to_receive[index1 + strlen(str1)], index2-index1-strlen(str1));

        // // Null-terminate the copied string
        // text_to_encrypt[index2-index1-strlen(str1)] = '\0';

        ESP_LOGI(TAG, "Begin encryption and decryption");

        // Encrypt and decrypt text
        // RSA_StartEncryptionTask();
        // vTaskDelay(pdMS_TO_TICKS(3000));
        RSA_StartDecryptionTask();
    }

    number_of_bytes_received = 0;
}

