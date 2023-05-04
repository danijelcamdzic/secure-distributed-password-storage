
/**
 * @file mqtt_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the MQTT311Client library 
 * and processing the data that has been received
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#ifndef MQTT_FUNCTIONS_H
#define MQTT_FUNCTIONS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

/* MQTT connection specific defines */
#define BROKER_ADDRESS              "mqtt.eclipseprojects.io"
#define BROKER_PORT_TCP             1883
#define BROKER_PORT_SSL             8883
#define KEEP_ALIVE                  600
#define CLIENT_ID                   "hardware_node_1"

/* Topics for subscribing and publishing the data */
#define SUB_TOPIC                   "/topic/pub/hw_node_1"          /**< On this topic the encrypted data is received */
#define PUB_TOPIC                   "/topic/sub/hw_node_1"          /**< On this topic the data is sent back */
#define ALL_TOPIC                   "/topic/pub/all"                /**< On this topic the command to send data back is received */

/* Data processing and confirmation defines */
#define END_MESSAGE_FLAG            "END_MESSAGE"
#define RECEPTION_CONFIRMATION      "OK"
#define RECEPTION_CONFIRMATION_SIZE 2

/* MQTT utility function prototypes */
int mqtt_find_substring_index(const char *substr, size_t substr_len);
void mqtt_receive_passphrase(int index_start, int index_end);
void mqtt_send_passphrase(void);
void mqtt_process_buffer_data(void);

#endif /* MQTT_FUNCTIONS_H */