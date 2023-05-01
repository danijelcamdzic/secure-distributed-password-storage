/**
 * @file tcp_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the tcp sockets in esp32
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
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

/* Defines for configuring the TCP Socket for IPv4 */
#ifndef CONFIG_EXAMPLE_IPV4
#define CONFIG_EXAMPLE_IPV4
#endif

/* Variable to be used as tcp socket */
extern int sock;

/* TCP function prototypes */
void tcp_connect_socket(const char* brokerAddress, uint16_t port);
void tcp_send_data(const char* data, uint16_t size);
void tcp_receive_data(void);

#endif /* TCP_FUNCTIONS_H */