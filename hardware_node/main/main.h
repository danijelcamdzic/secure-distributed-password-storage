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

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define END_MESSAGE_FLAG            "END_MESSAGE"
#define SUB_TOPIC                   "/topic/pub/hw_node_1"
#define PUB_TOPIC                   "/topic/sub/hw_node_1"
#define ALL_TOPIC                   "/topic/pub/all"

#define STORAGE_NAMESPACE "pass_space"

/* TCP Socket */
extern int sock;

/* Store in NVS and read from NVS function prototypes */
void store_in_nvs(const char* key, const void* value, size_t length);
void* read_from_nvs(const char* key);

/* TCP Socket function prototypes */
void connect_tcp_socket(const char* brokerAddress, uint16_t port);
void send_tcp_data(const char* data, uint16_t size);
void receive_tcp_data(void);

/* MQTT Receive Message Processing function prototypes */
int find_substring_index(const char *substr, size_t substr_len);
void receive_passphrase(int index_start, int index_end);
void send_passphrase(void);
void process_buffer_data(void);

/* Debug function prototypes */
void debug_print(char* message);
#endif /* MAIN_H */