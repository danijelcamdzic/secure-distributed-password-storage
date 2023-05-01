/**
 * @file tcp_functions.h
 * @brief Contains prototypes for helper functions for working with tcp sockets
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef TCP_FUNCTIONS_H
#define TCP_FUNCTIONS_H

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

/* TCP Socket */
extern int sock;

/* TCP Socket function prototypes */
void tcp_connect_socket(const char* brokerAddress, uint16_t port);
void tcp_send_data(const char* data, uint16_t size);
void tcp_receive_data(void);

#endif /* TCP_FUNCTIONS_H */