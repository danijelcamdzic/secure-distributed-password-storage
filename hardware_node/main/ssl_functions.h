/**
 * @file ssl_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the ssl sockets in esp32
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#ifndef SSL_FUNCTIONS_H
#define SSL_FUNCTIONS_H

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
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"

/* Defines for configuring the SSL Socket for IPv4 */
#ifndef CONFIG_EXAMPLE_IPV4
#define CONFIG_EXAMPLE_IPV4
#endif

/* SSL certificate */
extern const unsigned char *certificate;

/* SSL function prototypes */
void ssl_connect_socket(const char* brokerAddress, uint16_t port);
void ssl_send_data(const char* data, uint16_t size);
void ssl_receive_data(void);

#endif /* SSL_FUNCTIONS_H */
