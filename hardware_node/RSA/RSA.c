/**
 * @file RSA.h
 * @brief Contains tasks and task creation for RSA encryption
 * and decryption
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

/* RSA utility library */
#include "RSA/RSA.h"

/* ------------------------- VARIABLE DEFINITIONS ------------------------------------ */

/* Buffer to hold decrypted message */
unsigned char encrypted_text[MBEDTLS_MPI_MAX_SIZE] = {0};

/* Buffer to hold messages to encrypt */
char* text_to_encrypt;

/* Value to hold the encrypted message length */
size_t message_length = 0;

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief RSA encryption task
 *
 * @return None.
 */
void RSA_EncryptionTask(void *pvParameter)
{
    /* Always encrypt with the master public key (of the software node) */
    RSA_MESSAGE_LENGTH = RSA_Encrypt(RSA_MESSAGE_TO_ENCRYPT, RSA_MESSAGE_LENGTH, RSA_MASTER_PUBLIC_KEY);

    /* Delete the task */
    vTaskDelete(NULL);
}

/**
 * @brief RSA encryption task
 *
 * @return None.
 */
void RSA_DecryptionTask(void *pvParameter)
{
    /* Always decrypt with the private key of the hardware device */
    RSA_MESSAGE_LENGTH = RSA_Decrypt((char*)RSA_ENCRYPTED_BUFFER, RSA_MESSAGE_LENGTH, RSA_PRIVATE_KEY);

    /* Delete the task */
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
