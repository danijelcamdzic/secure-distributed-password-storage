/***********************************************************************
* FILENAME:        MQTT311_Publish.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 PUBLISH package.
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
#include "MQTT311.h"

/* Private function declaration */
static void MQTT311_AppendMessagePayload(const char* message_payload);
static void MQTT311_PublishWithStruct(struct PUBLISH_MESSAGE *publish_message_data);

/*
 * Function: MQTT311_AppendMessagePayload
 * ----------------------------
 *   Appends data payload to the message
 *
 *   message_payload: message payload - data
 *
 *   returns: no return value
 */
static void MQTT311_AppendMessagePayload(const char* message_payload) 
{
    uint16_t messagePayloadLength = strlen(message_payload);

    MQTT311_AppendData(message_payload, messagePayloadLength, false);

}

/*
 * Function: MQTT311_PublishWithStruct
 * ----------------------------
 *   Publishes message to Cumulocity using data from the structure of publish message.
 *
 *   publish_message_data: publish message structure
 *
 *   returns: no return value
 */
static void MQTT311_PublishWithStruct(struct PUBLISH_MESSAGE *publish_message_data) 
{
    current_index = 0;

    /* Appending PUBLISH packet type*/
    bytes_to_send[current_index++] = publish_message_data->packet_type;

    uint8_t header_flags = (publish_message_data->dup << DUP_FLAG) | (publish_message_data->qos1 << QOS_LEVEL1) |
                            (publish_message_data->qos2 << QOS_LEVEL2) | (publish_message_data->retain << RETAIN);

    /* Append the header flags */                       
    bytes_to_send[current_index] |= header_flags;

    /* Remaining size so far is 0 */
    bytes_to_send[current_index++] = publish_message_data->remaining_length;

    /* Appending topic name, packet identifier and payload */
    MQTT311_AppendTopicName(publish_message_data->topicName);

    /* Detecting the necessity for packet identifier */
    bool packet_id_presence = publish_message_data->qos1 | publish_message_data->qos2;

    /* Filling up the structure - Packet Identifier */
    if (packet_id_presence) 
    {
        bytes_to_send[current_index++] = publish_message_data->packetIdentifier >> 8;
        bytes_to_send[current_index++] = publish_message_data->packetIdentifier & 0xFF;
    }

    if (strcmp(publish_message_data->payload, "") != 0)
    {
        MQTT311_AppendMessagePayload(publish_message_data->payload);
    }
    
    /* Encode remaining length if larger than 127 */
    publish_message_data->remaining_length = MQTT311_CheckRemainingLength();

    /* Append remaining size */
    bytes_to_send[1] = publish_message_data->remaining_length;

    bool redelivery_flag = false;

    while(!redelivery_flag) 
    {
        /* Send data to server */
        MQTT311_SendToMQTTBroker(current_index);

        redelivery_flag = true;

        // /* Check puback only if QoS == 1 */
        // if((publish_message_data->qos1) & !(publish_message_data->qos2))
        // {
        //     redelivery_flag = get_puback(publish_message_data->packetIdentifier);

        //     if(!redelivery_flag)
        //     {
        //         printf("\r\nRepublishing package...\r\n");
        //         msleep(3000);

        //         /* Setting re-delivery flag */
        //         bytes_to_send[1] |= (1 << DUP_FLAG);
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }
        // /* Check pubrec only if QoS == 2 */
        // else if(!(publish_message_data->qos1) & (publish_message_data->qos2))
        // {
        //     redelivery_flag = get_puback(publish_message_data->packetIdentifier);
            
        //     /* Get correct order of ack messages from the server */
        //     /* If puback was received */
        //     if(redelivery_flag)
        //     {
        //         redelivery_flag = get_pubrec(publish_message_data->packetIdentifier);
        //     }
            
        //     /* If pubrec was received */
        //     if(redelivery_flag)
        //     {
        //         redelivery_flag = get_pubrel(publish_message_data->packetIdentifier);
        //     }

        //     /* If pubrel was received */
        //     if(redelivery_flag)
        //     {
        //         redelivery_flag = get_pubcomp(publish_message_data->packetIdentifier);
        //     }
            
        //     /* If pubcomp was received */
        //     if(!redelivery_flag)
        //     {
        //         printf("\r\nRepublishing package...\r\n");
        //         msleep(3000);

        //         /* Setting re-delivery flag */
        //         bytes_to_send[1] |= (1 << DUP_FLAG);
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }
        // else
        // {
        //     break;
        // }
    }
    /* Free dinamically allocated memory */
    vPortFree(publish_message_data->topicName);
    vPortFree(publish_message_data->payload);
    vPortFree(publish_message_data);
}

/*
 * Function: publish
 * ----------------------------
 *   Publishes message to Cumulocity.
 *
 *   header_flags: bits 0-3 of the header byte that can be configured
 *   topicName: name of the topic that gets published
 *   packetIdentifier: identifier of the packet. Possible only when QoS == 1 or QoS == 2
 *   payload: data that gets published
 *
 *   returns: no return value
 */
void MQTT311_Publish(uint8_t header_flags, const char* topicName, uint16_t packetIdentifier, const char* payload)
{
    /* Creating a PUBLISH_MESSAGE structure */
    struct PUBLISH_MESSAGE* publish_message_data = (struct PUBLISH_MESSAGE*) pvPortMalloc(sizeof *publish_message_data);
    NULL_CHECK(publish_message_data)

    /* Filling up the structure - Header Data */
    publish_message_data->packet_type = PUBLISH;

    /* Filling up the structure - Header Flags */
    publish_message_data->dup = ((header_flags & (1 << DUP_FLAG)) != 0);
    publish_message_data->qos1 = ((header_flags & (1 << QOS_LEVEL1)) != 0);
    publish_message_data->qos2 = ((header_flags & (1 << QOS_LEVEL2)) != 0);
    publish_message_data->retain = ((header_flags & (1 << RETAIN)) != 0);

    /* Filling up the structure - Topic Name */
    publish_message_data->topicName = (char*) pvPortMalloc(strlen(topicName)+1);
    NULL_CHECK(publish_message_data->topicName)
    memcpy(publish_message_data->topicName, topicName, strlen(topicName)+1);

    /* Detecting the necessity for packet identifier */
    bool packet_id_presence = publish_message_data->qos1 | publish_message_data->qos2;

    /* Filling up the structure - Packet Identifier */
    if (packet_id_presence) 
    {
        publish_message_data->packetIdentifier = packetIdentifier;
    }
    else 
    {
        publish_message_data->packetIdentifier = 0x00;
    }

    /* Filling up the structure - Payload*/
    publish_message_data->payload = (char*) pvPortMalloc(strlen(payload)+1);
    NULL_CHECK(publish_message_data->payload)
    memcpy(publish_message_data->payload, payload, strlen(payload)+1);

    /* Connecting a function */
    publish_message_data->MQTT311_PublishWithStruct = &MQTT311_PublishWithStruct;

    /* Creating a MQTTPacket and adding PUBLISH packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = ePUBLISH;
    mqtt_packet->packet_data.publish_message_structure = publish_message_data;
    
    /* Send to queue for the sending task to receive */
    // xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );

    MQTT311_PublishWithStruct(publish_message_data);
}