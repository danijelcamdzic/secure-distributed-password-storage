/**
 * @file MQTT311Client_Unsubscribe.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 UNSUBSCRIBE package.
 * 
 * @details This file contains the implementation of the MQTT 3.1.1 UNSUBSCRIBE package. 
 * It includes the definition of the UNSUBSCRIBE packet structure, as well as functions 
 * for encoding and decoding UNSUBSCRIBE packets.
 * 
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Unsubscribe.h"
#include "MQTT311Client/MQTT311Client_Unsuback.h"

/* Private function declaration */
static UnsubscribeMessageResult_t MQTT311Client_UnsubscribeWithStruct(struct UNSUBSCRIBE_MESSAGE *unsubscribe_message_data);

/**
 * @brief Unsubscribes from a topic using the provided unsubscribe message structure.
 *
 * This function is used to unsubscribe from a topic by sending an unsubscribe message with the 
 * provided unsubscribe message structure. The function takes the structure as input and does not 
 * return any value.
 * 
 * @param unsubscribe_message_data The unsubscribe message structure.
 *
 * @return UnsubscribeMessageResult_t.
 */ 
static UnsubscribeMessageResult_t MQTT311Client_UnsubscribeWithStruct(struct UNSUBSCRIBE_MESSAGE *unsubscribe_message_data)
{
    current_index = 0;

    /* Appending SUBSCRIBE packet type*/
    MQTT311_SEND_BUFFER[current_index++] = unsubscribe_message_data->packet_type | UNSUB_RESERVED;

    /* Remaining size so far is 0 */
    MQTT311_SEND_BUFFER[current_index++] = unsubscribe_message_data->remaining_length;

    /* Append packet identifier */
    MQTT311_SEND_BUFFER[current_index++] = unsubscribe_message_data->packet_identifier >> 8;
    MQTT311_SEND_BUFFER[current_index++] = unsubscribe_message_data->packet_identifier & 0xFF;

    MQTT311Client_AppendTopicName(unsubscribe_message_data->topic_name);

    /* Encode remaining length if larger than 127 */
    MQTT311Client_CheckRemainingLength();

    uint32_t redelivery_attempts = 0;

    while(redelivery_attempts < REDELIVERY_ATTEMPTS_MAX)
    {
        /* Send data to server */
        MQTT311Client_SendToMQTTBroker(current_index);

        /* Read the acknowledge */
        if(MQTT311Client_Unsuback(unsubscribe_message_data->packet_identifier))
        {
            MQTT311Client_Print("Successfull unsubscribing!");
            break;
        }
        else 
        {
            MQTT311Client_Print("Unsuccesfull unsubscription!");
            vTaskDelay(pdMS_TO_TICKS(2000));

            redelivery_attempts++;
        }
    }

    vPortFree(unsubscribe_message_data->topic_name);
    vPortFree(unsubscribe_message_data);

    UnsubscribeMessageResult_t unsubscribe_result = (redelivery_attempts < REDELIVERY_ATTEMPTS_MAX) ? UNSUBSCRIBE_SUCCESS:UNSUBSCRIBE_FAIL;

    return unsubscribe_result;
}

/**
 * @brief Unsubscribes from a topic with the given name and packet ID.
 *
 * This function is used to unsubscribe from a topic with the given name and packet ID. The function
 * does not return any value.
 * 
 * @param packet_id The packet identifier (2 bytes).
 * @param topicName The name of the topic that will be unsubscribed from.
 *
 * @return None.
 */ 
void MQTT311Client_Unsubscribe(uint16_t packet_id, const char* topicName)
{
    
    /* Creating a UNSUBSCRIBE_MESSAGE structure */
    struct UNSUBSCRIBE_MESSAGE* unsubscribe_message_data = (struct UNSUBSCRIBE_MESSAGE*) pvPortMalloc(sizeof *unsubscribe_message_data);
    NULL_CHECK(unsubscribe_message_data)

    /* Filling up the structure - Header Data */
    unsubscribe_message_data->packet_type = UNSUBSCRIBE;

    /* Filling up the structure - Rest of the data */
    unsubscribe_message_data->packet_identifier = packet_id;

    unsubscribe_message_data->topic_name = (char*) pvPortMalloc(strlen(topicName)+1);
    NULL_CHECK(unsubscribe_message_data->topic_name)
    memcpy(unsubscribe_message_data->topic_name, topicName, strlen(topicName)+1);

    /* Connecting a function */
    unsubscribe_message_data->MQTT311Client_UnsubscribeWithStruct = &MQTT311Client_UnsubscribeWithStruct;

    /* Creating a MQTTPacket and adding SUBSCRIBE packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = eUNSUBSCRIBE;
    mqtt_packet->packet_data.unsubscribe_message_structure = unsubscribe_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );
}

