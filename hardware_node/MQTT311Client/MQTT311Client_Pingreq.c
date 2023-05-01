/**
 * @file MQTT311Client_Pingreq.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PINGREQ package.
 *
 * This file contains the variables and function definitions required for implementing the MQTT 3.1.1 PINGREQ package.
 * The PINGREQ package is used to keep the MQTT connection alive by periodically sending a ping request to the broker.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Pingreq.h"
#include "MQTT311Client/MQTT311Client_Pingresp.h"

/* Private function declaration */
static PingreqMessageResult_t MQTT311Client_PingreqWithStruct(struct PINGREQ_MESSAGE *pingreq_message_data);

/**
 * @brief Sends a PINGREQ message to the MQTT broker using the provided message structure.
 *
 * This function sends a PINGREQ message to the MQTT broker using the provided message structure.
 * The message structure is passed in as a pointer to a `PINGREQ_MESSAGE` structure.
 *
 * @param pingreq_message_data Pointer to a `PINGREQ_MESSAGE` structure containing the PINGREQ message.
 *
 * @return PingreqMessageResult_t.
 */
static PingreqMessageResult_t MQTT311Client_PingreqWithStruct(struct PINGREQ_MESSAGE *pingreq_message_data)
{
    current_index = 0;

    /* Appending SUBSCRIBE packet type*/
    MQTT311_SEND_BUFFER[current_index++] = pingreq_message_data->packet_type;

    /* Remaining size so far is 0 */
    MQTT311_SEND_BUFFER[current_index++] = pingreq_message_data->remaining_length;

    pingreq_message_data->remaining_length = current_index - 2;

    /* Append remaining size */
    MQTT311_SEND_BUFFER[1] = pingreq_message_data->remaining_length;

    uint32_t redelivery_attempts = 0;

    while(redelivery_attempts < REDELIVERY_ATTEMPTS_MAX)
    {
        /* Send data to server */
        MQTT311Client_SendToMQTTBroker(current_index);
        
        /* Read the acknowledge */
        if(MQTT311Client_Pingresp())
        {
            MQTT311Client_Print("Successfull pinging!");
            break;
        }
        else 
        {
            MQTT311Client_Print("Unsuccesfull pinging!");
            vTaskDelay(pdMS_TO_TICKS(2000));
            redelivery_attempts++;
        }
    }

    PingreqMessageResult_t pingreq_result = (redelivery_attempts < REDELIVERY_ATTEMPTS_MAX) ? PINGREQ_SUCCESS:PINGREQ_FAIL;

    return pingreq_result;
}

/**
 * @brief Sends a PINGREQ message to the MQTT broker.
 *
 * This function sends a PINGREQ message to the MQTT broker. The PINGREQ message is used to keep the MQTT connection
 * alive by periodically sending a ping request to the broker.
 *
 * @return None.
 */
void MQTT311Client_Pingreq(void)
{
    /* Creating a PINGREQ_MESSAGE structure */
    struct PINGREQ_MESSAGE* pingreq_message_data = (struct PINGREQ_MESSAGE*) pvPortMalloc(sizeof *pingreq_message_data);
    NULL_CHECK(pingreq_message_data)

    /* Filling up the structure - Header Data */
    pingreq_message_data->packet_type = PINGREQ;

    /* Filling up the structure - Rest of the data */
    pingreq_message_data->remaining_length = 0x00;

    /* Connecting a function */
    pingreq_message_data->MQTT311Client_PingreqWithStruct = &MQTT311Client_PingreqWithStruct;

    /* Creating a MQTTPacket and adding SUBSCRIBE packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = ePINGREQ;
    mqtt_packet->packet_data.pingreq_message_structure = pingreq_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );
}
