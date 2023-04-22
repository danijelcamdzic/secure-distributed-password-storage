/**
 * @file MQTT311Client_Publish.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PUBLISH package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client.h"

/* Private function declaration */
static void MQTT311Client_AppendMessagePayload(const char* message_payload);
static void MQTT311Client_PublishWithStruct(struct PUBLISH_MESSAGE *publish_message_data);

/**
 * @brief Appends data payload to the message
 *
 * @param message_payload Message payload - data
 *
 * @return None
 */
static void MQTT311Client_AppendMessagePayload(const char* message_payload) 
{
    uint16_t messagePayloadLength = strlen(message_payload);

    MQTT311Client_AppendData(message_payload, messagePayloadLength, false);
}

/**
 * @brief Publishes message to MQTT Broker using data from the structure of publish message.
 *
 * @param publish_message_data Pointer to the publish message structure
 *
 * @return None
 */
static void MQTT311Client_PublishWithStruct(struct PUBLISH_MESSAGE *publish_message_data) 
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
    MQTT311Client_AppendTopicName(publish_message_data->topicName);

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
        MQTT311Client_AppendMessagePayload(publish_message_data->payload);
    }
    
    /* Encode remaining length if larger than 127 */
    MQTT311Client_CheckRemainingLength();

    bool redelivery_flag = false;

    while(!redelivery_flag) 
    {
        /* Send data to server */
        MQTT311Client_SendToMQTTBroker(current_index);

        /* Check puback only if QoS == 1 */
        if((publish_message_data->qos1) & !(publish_message_data->qos2))
        {
            redelivery_flag = MQTT311Client_Puback(publish_message_data->packetIdentifier);

            if(!redelivery_flag)
            {
                MQTT311Client_Print("Republishing package...");
                vTaskDelay(pdMS_TO_TICKS(3000));

                /* Setting re-delivery flag */
                bytes_to_send[1] |= (1 << DUP_FLAG);
            }
            else
            {
                break;
            }
        }
        /* Check pubrec only if QoS == 2 */
        else if(!(publish_message_data->qos1) & (publish_message_data->qos2))
        {
            redelivery_flag = MQTT311Client_Puback(publish_message_data->packetIdentifier);
            
            /* Get correct order of ack messages from the server */
            /* If puback was received */
            if(redelivery_flag)
            {
                redelivery_flag = MQTT311Client_Pubrec(publish_message_data->packetIdentifier);
            }
            
            /* If pubrec was received */
            if(redelivery_flag)
            {
                redelivery_flag = MQTT311Client_Pubrel(publish_message_data->packetIdentifier);
            }

            /* If pubrel was received */
            if(redelivery_flag)
            {
                redelivery_flag = MQTT311Client_Pubcomp(publish_message_data->packetIdentifier);
            }
            
            /* If pubcomp was received */
            if(!redelivery_flag)
            {
                MQTT311Client_Print("Republishing package...");
                vTaskDelay(pdMS_TO_TICKS(3000));

                /* Setting re-delivery flag */
                bytes_to_send[1] |= (1 << DUP_FLAG);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    /* Free dinamically allocated memory */
    vPortFree(publish_message_data->topicName);
    vPortFree(publish_message_data->payload);
    vPortFree(publish_message_data);
}

/**
 * @brief Publishes message to MQTT Broker.
 *
 * @param header_flags Bits 0-3 of the header byte that can be configured
 * @param topicName Name of the topic that gets published
 * @param packetIdentifier Identifier of the packet. Possible only when QoS == 1 or QoS == 2
 * @param payload Data that gets published
 *
 * @return None
 */
void MQTT311Client_Publish(uint8_t header_flags, const char* topicName, uint16_t packetIdentifier, const char* payload)
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
    publish_message_data->MQTT311Client_PublishWithStruct = &MQTT311Client_PublishWithStruct;

    /* Creating a MQTTPacket and adding PUBLISH packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = ePUBLISH;
    mqtt_packet->packet_data.publish_message_structure = publish_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );
}

