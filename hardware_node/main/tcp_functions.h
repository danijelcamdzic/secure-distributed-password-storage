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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include <netdb.h>
#include <fcntl.h>
#include "lwip/err.h"
#include "lwip/sys.h"

/* TCP function prototypes */
void tcp_connect_socket(const char* brokerAddress, uint16_t port);
void tcp_send_data(const char* data, uint16_t size);
void tcp_receive_data(void);

#endif /* TCP_FUNCTIONS_H */