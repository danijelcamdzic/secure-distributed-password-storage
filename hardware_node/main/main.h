/**
 * @file main.h
 * @brief Contains prototypes for helper functions
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MAIN_H
#define MAIN_H

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

/* Debug function prototypes */
void debug_print(char* message);

#endif /* MAIN_H */