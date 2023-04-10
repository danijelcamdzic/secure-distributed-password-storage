/**
 * @file MQTT311_Disconnect.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 DISCONNECT package.
 *
 * This file contains the variables and function definitions required for implementing the MQTT 3.1.1 DISCONNECT package.
 * The DISCONNECT package is used to gracefully terminate an MQTT connection.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311/MQTT311.h"

/* Private function declaration */
static void MQTT311_DisconnectWithStruct(struct DISCONNECT_MESSAGE *disconnect_message_data);

/**
 * @brief Disconnects from the MQTT broker using the provided disconnect message.
 *
 * This function gracefully terminates the MQTT connection with the broker using the provided disconnect message.
 * The disconnect message is passed in as a pointer to a `DISCONNECT_MESSAGE` structure.
 *
 * @param disconnect_message_data Pointer to a `DISCONNECT_MESSAGE` structure containing the disconnect message.
 *
 * @return None.
 */
static void MQTT311_DisconnectWithStruct(struct DISCONNECT_MESSAGE *disconnect_message_data)
{
    current_index = 0;

    /* Appending SUBSCRIBE packet type*/
    bytes_to_send[current_index++] = disconnect_message_data->packet_type;

    /* Remaining size so far is 0 */
    bytes_to_send[current_index++] = disconnect_message_data->remaining_length;
    disconnect_message_data->remaining_length = current_index - 2;

    /* Append remaining size */
    bytes_to_send[1] = disconnect_message_data->remaining_length;

    /* Send data to server */
    MQTT311_SendToMQTTBroker(current_index);
}

/**
 * @brief Disconnects from the MQTT broker.
 *
 * This function gracefully terminates the MQTT connection with the broker.
 *
 * @return None.
 */
void MQTT311_Disconnect(void)
{
    /* Creating a PINGREQ_MESSAGE structure */
    struct DISCONNECT_MESSAGE* disconnect_message_data = (struct DISCONNECT_MESSAGE*) pvPortMalloc(sizeof *disconnect_message_data);
    NULL_CHECK(disconnect_message_data)

    /* Filling up the structure - Header Data */
    disconnect_message_data->packet_type = DISCONNECT;

    /* Filling up the structure - Rest of the data */
    disconnect_message_data->remaining_length = 0x00;

    /* Connecting a function */
    disconnect_message_data->MQTT311_DisconnectWithStruct = &MQTT311_DisconnectWithStruct;

    /* Creating a MQTTPacket and adding SUBSCRIBE packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = eDISCONNECT;
    mqtt_packet->packet_data.disconnect_message_structure = disconnect_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );
}