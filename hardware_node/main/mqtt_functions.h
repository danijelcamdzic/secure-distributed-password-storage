
/**
 * @file mqtt_functions.h
 * @brief Contains prototypes for helper functions for working with mqtt
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT_FUNCTIONS_H
#define MQTT_FUNCTIONS_H

/* MQTT Specific Defines */
#define BROKER_ADDRESS              "mqtt.eclipseprojects.io"
#define BROKER_PORT_TCP             1883
#define CLIENT_ID                   "client_id_dado"
#define END_MESSAGE_FLAG            "END_MESSAGE"
#define SUB_TOPIC                   "/topic/pub/hw_node_1"
#define PUB_TOPIC                   "/topic/sub/hw_node_1"
#define ALL_TOPIC                   "/topic/pub/all"

/* MQTT Receive Message Processing function prototypes */
int mqtt_find_substring_index(const char *substr, size_t substr_len);
void mqtt_receive_passphrase(int index_start, int index_end);
void mqtt_send_passphrase(void);
void mqtt_process_buffer_data(void);

#endif /* MQTT_FUNCTIONS_H */