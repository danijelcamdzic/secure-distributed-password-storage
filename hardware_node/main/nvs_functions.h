/**
 * @file nvs_functions.h
 * @brief Contains prototypes for helper functions for working with nvs
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef NVS_FUNCTIONS_H
#define NVS_FUNCTIONS_H

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

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

/* NVS Specific Defines */
#define STORAGE_NAMESPACE           "pass_space"
#define PASSWORD_STORE_KEY          "password"

/* NVS function prototypes */
esp_err_t nvs_init();
esp_err_t nvs_store_string(const char *key, const char *value);
esp_err_t nvs_read_string(const char *key, char *value, size_t *length);
void nvs_store(const char* key, const void* value, size_t length);
void* nvs_read(const char* key);

#endif /* NVS_FUNCTIONS_H */