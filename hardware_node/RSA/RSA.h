/**
 * @file RSA.h
 * @brief Contains libraries and definitions necessary for proper
 * proper functioning of the RSA cryptographic algorithm
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef RSA_H
#define RSA_H

/* Included libraries */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS library */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

/* MBED TLS library */
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/platform.h"
#include "mbedtls/base64.h"

/* Control type over which public key to use */
typedef enum PublicKeyControl_e {
    MASTER_PUBLIC_KEY = 0,
    DEVICE_PUBLIC_KEY
} PublicKeyControl_t;

/* Macros for accessing the internal variables */
#define RSA_ENCRYPTED_BUFFER    (encrypted_text)
#define RSA_MESSAGE_TO_ENCRYPT  (text_to_encrypt)
#define RSA_MESSAGE_LENGTH      (message_length)
#define RSA_MASTER_PUBLIC_KEY   (masterkey)
#define RSA_PUBLIC_KEY          (key)
#define RSA_PRIVATE_KEY         (private_key)

/* Master's Public RSA key */
extern const unsigned char *masterkey;

/* Public RSA key */
extern const unsigned char *key;

/* Private RSA key */
extern const unsigned char *private_key;

/* Buffer to hold messages */
extern unsigned char encrypted_text[MBEDTLS_MPI_MAX_SIZE];

/* Buffer to hold messages to encrypt */
extern char* text_to_encrypt;

/* Value to hold the encrypted message length */
extern size_t message_length;

/* Task priorities */
#define ENCRYPT_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define DECRYPT_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* Start tasks prototypes */
void RSA_StartEncryptionTask(void);
void RSA_StartDecryptionTask(void);

/* Encryption/Decryption task prototypes */
void RSA_EncryptionTask(void *pvParameter);
void RSA_DecryptionTask(void *pvParameter);

/* Encryption/Decryption function prototypes */
size_t RSA_Encrypt(const char* text, size_t length, const unsigned char* rsa_key);
size_t RSA_Decrypt(const char* text, size_t length, const unsigned char* rsa_private_key);

/* Utility functions */
/* External functions for debugging */
typedef void (*RSA_PrintPtr)(char*);
extern RSA_PrintPtr RSA_Print;
void RSA_SetPrint(RSA_PrintPtr print);

#endif /* RSA_H */

