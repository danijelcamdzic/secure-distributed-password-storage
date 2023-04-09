/***********************************************************************
* FILENAME:        MQTT311_Unsubscribe.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 UNSUBSCRIBE package.
*
* NOTES:
*       
*
* AUTHOR:          Danijel Camdzic     
*
*   
* DATE:            19 Aug 21
*
*
* CHANGES:
*
* VERSION:         DATE:          WHO:         DETAIL:
* 0.00.0           19 Aug 21      DC           Initial state of the file
*
*/

/* Included libraries */
#include "MQTT311/MQTT311.h"

/* Private function declaration */
static void MQTT311_UnsubscribeWithStruct(struct UNSUBSCRIBE_MESSAGE *unsubscribe_message_data);

/*
 * Function: MQTT311_UnsubscribeWithStruct
 * ----------------------------
 *   Unsubscribes from a topic.
 *
 *   unsubscribe_message_data: unsubscribe message structure
 *
 *   returns: no return value
 */
static void MQTT311_UnsubscribeWithStruct(struct UNSUBSCRIBE_MESSAGE *unsubscribe_message_data)
{
    current_index = 0;

    /* Appending SUBSCRIBE packet type*/
    bytes_to_send[current_index++] = unsubscribe_message_data->packet_type | UNSUB_RESERVED;

    /* Remaining size so far is 0 */
    bytes_to_send[current_index++] = unsubscribe_message_data->remaining_length;

    /* Append packet identifier */
    bytes_to_send[current_index++] = unsubscribe_message_data->packet_identifier >> 8;
    bytes_to_send[current_index++] = unsubscribe_message_data->packet_identifier & 0xFF;

    MQTT311_AppendTopicName(unsubscribe_message_data->topic_name);

    /* Encode remaining length if larger than 127 */
    unsubscribe_message_data->remaining_length = MQTT311_CheckRemainingLength();

    /* Append remaining size */
    bytes_to_send[1] = unsubscribe_message_data->remaining_length;

    /* Send data to server */
    MQTT311_SendToMQTTBroker(current_index);

    /* Read the acknowledge */
    if(MQTT311_Unsuback(unsubscribe_message_data->packet_identifier))
    {
        MQTT311_Print("Unsubscribe succesfull!");
    }
    else 
    {
        MQTT311_Print("Unsuccesfull unsubscription!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vPortFree(unsubscribe_message_data->topic_name);
    vPortFree(unsubscribe_message_data);
}

/*
 * Function: MQTT311_Unsubscribe
 * ----------------------------
 *   Unsubscribes from topic
 *
 *   packet_id: packet identifier (2 bytes)
 *   topicName: name of the topic that will be unsubscribed from
 *
 *   returns: no return value
 */
void MQTT311_Unsubscribe(uint16_t packet_id, const char* topicName)
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
    unsubscribe_message_data->MQTT311_UnsubscribeWithStruct = &MQTT311_UnsubscribeWithStruct;

    /* Creating a MQTTPacket and adding SUBSCRIBE packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = eUNSUBSCRIBE;
    mqtt_packet->packet_data.unsubscribe_message_structure = unsubscribe_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );
}