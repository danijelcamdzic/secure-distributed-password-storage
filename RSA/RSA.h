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

/* TO DELETE */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <netdb.h>
#include <fcntl.h>

/* Public RSA key */
extern const unsigned char *key;

/* Private RSA key */
extern const unsigned char *private_key;

/* Buffer to hold messages */
extern unsigned char buf[MBEDTLS_MPI_MAX_SIZE];

/* Buffer to hold messages to encrypt */
extern char* text_to_encrypt;

/* Value to hold the encrypted message length */
extern size_t length;

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
size_t RSA_Encrypt(const char* text);
void RSA_Decrypt(const char* text, size_t length);

/* Utility functions */
/* External functions for debugging */
typedef void (*RSA_PrintPtr)(char*);
extern RSA_PrintPtr RSA_Print;
void RSA_SetPrint(RSA_PrintPtr print);

#endif /* RSA_H */

