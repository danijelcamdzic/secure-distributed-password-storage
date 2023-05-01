/**
 * @file nvs_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the NVS in esp32
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#ifndef NVS_FUNCTIONS_H
#define NVS_FUNCTIONS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

/* NVS Specific Defines */
#define STORAGE_NAMESPACE           "pass_space"
#define PASSWORD_STORE_KEY          "password"

/* NVS function prototypes */
esp_err_t nvs_init();
void nvs_store(const char* key, const void* value, size_t length);
void* nvs_read(const char* key);

#endif /* NVS_FUNCTIONS_H */