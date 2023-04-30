/**
 * @file RSA.c
 * @brief Contains implementation of RSA encrypt and decrypt tasks
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* RSA library */
#include "RSA/RSA.h"

/* Buffer to hold decrypted message */
unsigned char encrypted_text[MBEDTLS_MPI_MAX_SIZE] = {0};

/* Buffer to hold messages to encrypt */
char* text_to_encrypt;

/* Value to hold the encrypted message length */
size_t message_length = 0;

/* Public RSA key to use */
const unsigned char *key_to_use = NULL;

/**
 * @brief RSA encryption task
 *
 * @return None.
 */
void RSA_EncryptionTask(void *pvParameter)
{
    for (int i = 0; i < RSA_MESSAGE_LENGTH; i++)
    {
        printf("%c ", RSA_MESSAGE_TO_ENCRYPT[i]);
    }

    if (RSA_PUBLIC_KEY_TO_USE != NULL) {
        RSA_MESSAGE_LENGTH = RSA_Encrypt(RSA_MESSAGE_TO_ENCRYPT, RSA_MASTER_PUBLIC_KEY);
        free(RSA_MESSAGE_TO_ENCRYPT);
    }
    else {
        RSA_Print("Public key to use is not defined!");
    }

    vTaskDelete(NULL);
}

/**
 * @brief RSA encryption task
 *
 * @return None.
 */
void RSA_DecryptionTask(void *pvParameter)
{
    RSA_MESSAGE_LENGTH = RSA_Decrypt((char*)RSA_ENCRYPTED_BUFFER, RSA_MESSAGE_LENGTH);

    vTaskDelete(NULL);
}

/**
 * @brief Starts RSA encryption task
 *
 * @return None.
 */
void RSA_StartEncryptionTask(void)
{
    xTaskCreate(RSA_EncryptionTask, "RSAEncryptTask", 8*configMINIMAL_STACK_SIZE, NULL, ENCRYPT_TASK_PRIORITY, NULL);
}

/**
 * @brief Starts RSA decryption task
 *
 * @return None.
 */
void RSA_StartDecryptionTask(void)
{
    xTaskCreate(RSA_DecryptionTask, "RSADecryptTask", 8*configMINIMAL_STACK_SIZE, NULL, DECRYPT_TASK_PRIORITY, NULL);
}