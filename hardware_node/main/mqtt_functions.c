/**
 * @file mqtt_functions.c
 * @brief Contains necessary defines, variables defintions
 * and function definitions for using the MQTT311Client library 
 * and processing the data that has been received
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

/* Utility headers for MQTT and NVS */
#include "mqtt_functions.h"
#include "nvs_functions.h"

/* MQTT311Client library header */
#include "MQTT311Client/MQTT311Client.h"
#include "MQTT311Client/MQTT311Client_Publish.h"

/* RSA utility library header */
#include "RSA/RSA.h"

/* ------------------------- VARIABLE DEFINITIONS ------------------------------------ */

/* SSL certificate */
const unsigned char *certificate = (const unsigned char *)"-----BEGIN CERTIFICATE-----\n"
                                                        "MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n"
                                                        "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
                                                        "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n"
                                                        "WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
                                                        "RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
                                                        "AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n"
                                                        "R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n"
                                                        "sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n"
                                                        "NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n"
                                                        "Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n"
                                                        "/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n"
                                                        "AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n"
                                                        "Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n"
                                                        "FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n"
                                                        "AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n"
                                                        "Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n"
                                                        "gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n"
                                                        "PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n"
                                                        "ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n"
                                                        "CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n"
                                                        "lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n"
                                                        "avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n"
                                                        "yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n"
                                                        "yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n"
                                                        "hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n"
                                                        "HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n"
                                                        "MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n"
                                                        "nLRbwHOoq7hHwg==\n"
                                                        "-----END CERTIFICATE-----\n";

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

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
int mqtt_find_substring_index(const char *substr, size_t substr_len) 
{
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
 * @brief Receive a passphrase, decrypt it with private key, encrypt it with master key and store it
 *
 * This function reads a passphrase from the MQTT311_RECEIVE_BUFFER starting from
 * index_start and ending at index_end (exclusive). The passphrase is then decrypted in order
 * to encrypt it with the master key and store it like that in the NVS. After finishing, the 
 * confirmation message is sent.
 *
 * @param index_start The starting index of the passphrase in the MQTT311_RECEIVE_BUFFER.
 * @param index_end The ending index (exclusive) of the passphrase in the MQTT311_RECEIVE_BUFFER.
 */
void mqtt_receive_passphrase(int index_start, int index_end)
{
    char* TAG = "mqtt_receive_passphrase";                                  /**< Declare and initialize TAG for logging purposes */

    for (int i = index_start; i < index_end; i++)
    {
        RSA_ENCRYPTED_BUFFER[i - index_start] = MQTT311_RECEIVE_BUFFER[i];
    }

    /* Set the received data length */
    RSA_MESSAGE_LENGTH = index_end - index_start;

#ifdef DEBUG
    /* Check the received data length */
    ESP_LOGI(TAG, "Received message to decrypt length: %d bytes", RSA_MESSAGE_LENGTH);
#endif

    /* Decrypt the data received with hardware private key */
    ESP_LOGI(TAG, "Begin decryption of received data...");
    RSA_StartDecryptionTask();
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Finished decryption...");

#ifdef DEBUG
    /* Check length after decryption */
    ESP_LOGI(TAG, "Length of the decrypted message: %d bytes", RSA_MESSAGE_LENGTH);
#endif

    /* Begin encryption of the data with master public key */
    ESP_LOGI(TAG, "Begin encryption of received data...");
    free(RSA_MESSAGE_TO_ENCRYPT);
    RSA_MESSAGE_TO_ENCRYPT = malloc(RSA_MESSAGE_LENGTH);
    memcpy(RSA_MESSAGE_TO_ENCRYPT, RSA_ENCRYPTED_BUFFER, RSA_MESSAGE_LENGTH);
    RSA_StartEncryptionTask();
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Finished encryption...");

#ifdef DEBUG
    /* Check the encryption data length */
    ESP_LOGI(TAG, "Message length after encryption: %d bytes", RSA_MESSAGE_LENGTH);
#endif

    /* Store the data into NVS */
    ESP_LOGI(TAG, "Begin storing data in NVS...");
    const char *pass_key = PASSWORD_STORE_KEY;
    char* password = malloc(RSA_MESSAGE_LENGTH);
    /* Store the password piece from the RSA_ENCRYPTED_BUFFER to temporary memory */
    memcpy(password, RSA_ENCRYPTED_BUFFER, RSA_MESSAGE_LENGTH);
    nvs_store(pass_key, password, RSA_MESSAGE_LENGTH);
    ESP_LOGI(TAG, "Finished storing data in NVS...");
    
    /* Publish the OK message */
    MQTT311Client_Publish(0x00, PUB_TOPIC, 0x00, RECEPTION_CONFIRMATION, RECEPTION_CONFIRMATION_SIZE);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "Published the OK message...");
}

/**
 * @brief Send the encrypted passphrase to master
 *
 * This function reads the encrypted value from NVMem and sends it to MQTT311 topic.
 *
 * @param None
 */
void mqtt_send_passphrase()
{
    char* TAG = "mqtt_send_passphrase";                                  /**< Declare and initialize TAG for logging purposes */  

    ESP_LOGI(TAG, "Begin publishing the passphrase...");
    /* Read the value from NVS */
    const char *pass_key = PASSWORD_STORE_KEY;
    char* read_value = nvs_read(pass_key);

    /* Publish the read value */
    MQTT311Client_Publish(0x00, PUB_TOPIC, 0x00, read_value, RSA_ENCRYPT_SIZE);
    vTaskDelay(pdMS_TO_TICKS(3000));

    free(read_value);                                                   /**< Free allocated memory */
    ESP_LOGI(TAG, "Finished publishing the passphrase...");
}

/**
 * @brief Search for commands within the MQTT311_RECEIVE_BUFFER
 *
 * This function searches for commands within the MQTT311_RECEIVE_BUFFER and 
 * does appropriate actions according to it. It either receives the passphrase and stores it in NVS
 * or sends it from NVS to some MQTT311 topic
 *
 * @param None
 */
void mqtt_process_buffer_data(void) 
{
    char* TAG = "mqtt_process_buffer_data";                             /**< Declare and initialize TAG for logging purposes */   

    /* Search for the command to send the encrypted passphrase */
    char* str1 = ALL_TOPIC;
    char* str2 = END_MESSAGE_FLAG;

    int index_start = mqtt_find_substring_index(str1, strlen(str1));
    int index_end = mqtt_find_substring_index(str2, strlen(str2));

    if ((index_start != -1) && (index_end != -1)) {
        ESP_LOGI(TAG, "Received command to send passphrase!");
        mqtt_send_passphrase();

        /* Reset the buffer and return */
        MQTT311_RECEIVED_BYTES = 0;
        return;
    }

    /* Search for the command to store the encrypted passphrase */
    char* str3 = SUB_TOPIC;
    char* str4 = END_MESSAGE_FLAG;

    index_start = mqtt_find_substring_index(str3, strlen(str3));
    index_end = mqtt_find_substring_index(str4, strlen(str4));

    if ((index_start != -1) && (index_end != -1)) {
        ESP_LOGI(TAG, "Received command to store password!");
        mqtt_receive_passphrase(index_start + strlen(str3), index_end);
    }

    /* Reset the buffer */
    MQTT311_RECEIVED_BYTES = 0;
}
