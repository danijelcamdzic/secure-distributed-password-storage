/**
 * @file MQTT311Client_Subscribe.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 SUBSCRIBE package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client.h"

/* Private functions */
static void MQTT311Client_SubscribeWithStruct(struct SUBSCRIBE_MESSAGE *subscribe_message_data);

/**
 * @brief Subscribes to topic using data from the structure of subscribe message.
 *
 * @param subscribe_message_data Subscribe message structure
 *
 * @return None
 */
static void MQTT311Client_SubscribeWithStruct(struct SUBSCRIBE_MESSAGE *subscribe_message_data)
{
    current_index = 0;

    /* Appending SUBSCRIBE packet type*/
    MQTT311_SEND_BUFFER[current_index++] = subscribe_message_data->packet_type | SUB_RESERVED;

    /* Remaining size so far is 0 */
    MQTT311_SEND_BUFFER[current_index++] = subscribe_message_data->remaining_length;

    /* Append packet identifier */
    MQTT311_SEND_BUFFER[current_index++] = subscribe_message_data->packet_identifier >> 8;
    MQTT311_SEND_BUFFER[current_index++] = subscribe_message_data->packet_identifier & 0xFF;

    MQTT311Client_AppendTopicName(subscribe_message_data->topic_name);
    
    /* Append requested qos */
    MQTT311_SEND_BUFFER[current_index++] = subscribe_message_data->requested_qos;

    /* Encode remaining length if larger than 127 */
    MQTT311Client_CheckRemainingLength();

    /* Send data to server */
    MQTT311Client_SendToMQTTBroker(current_index);

    /* Read the acknowledge */
    if(MQTT311Client_Suback(subscribe_message_data->packet_identifier))
    {
        MQTT311Client_Print("Success");
    }
    else 
    {
        MQTT311Client_Print("Unsuccesfull subscription!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vPortFree(subscribe_message_data->topic_name);
    vPortFree(subscribe_message_data);
}

/**
 * @brief Subscribes to a topic.
 *
 * @param packet_id Packet identifier (2 bytes)
 * @param topic_name Name of the topic(s) to subscribe to
 * @param requested_qos Requested QoS level
 *
 * @return None
 */
void MQTT311Client_Subscribe(uint16_t packet_id, const char* topic_name, uint8_t requested_qos)
{
    
    /* Creating a SUBSCRIBE_MESSAGE structure */
    struct SUBSCRIBE_MESSAGE* subscribe_message_data = (struct SUBSCRIBE_MESSAGE*) pvPortMalloc(sizeof *subscribe_message_data);
    NULL_CHECK(subscribe_message_data)

    /* Filling up the structure - Header Data */
    subscribe_message_data->packet_type = SUBSCRIBE;

    /* Filling up the structure - Rest of the data */
    subscribe_message_data->packet_identifier = packet_id;

    subscribe_message_data->topic_name = (char*) pvPortMalloc(strlen(topic_name)+1);
    NULL_CHECK(subscribe_message_data->topic_name)
    memcpy(subscribe_message_data->topic_name, topic_name, strlen(topic_name)+1);

    subscribe_message_data->requested_qos = requested_qos;

    /* Connecting a function */
    subscribe_message_data->MQTT311Client_SubscribeWithStruct = &MQTT311Client_SubscribeWithStruct;

    /* Creating a MQTTPacket and adding SUBSCRIBE packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = eSUBSCRIBE;
    mqtt_packet->packet_data.subscribe_message_structure = subscribe_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );
}

