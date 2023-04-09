/***********************************************************************
* FILENAME:        MQTT311_Pingreq.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 PINGREQ package.
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
static void MQTT311_PingreqWithStruct(struct PINGREQ_MESSAGE *pingreq_message_data);

/*
 * Function: MQTT311_PingreqWithStruct
 * ----------------------------
 *   Pings the server.
 *
 *   pingreq_message_data: pingreq message structure
 *
 *   returns: no return value
 */
static void MQTT311_PingreqWithStruct(struct PINGREQ_MESSAGE *pingreq_message_data)
{
    current_index = 0;

    /* Appending SUBSCRIBE packet type*/
    bytes_to_send[current_index++] = pingreq_message_data->packet_type;

    /* Remaining size so far is 0 */
    bytes_to_send[current_index++] = pingreq_message_data->remaining_length;

    pingreq_message_data->remaining_length = current_index - 2;

    /* Append remaining size */
    bytes_to_send[1] = pingreq_message_data->remaining_length;

    /* Send data to server */
    MQTT311_SendToMQTTBroker(current_index);

    /* Read the acknowledge */
    if(MQTT311_Pingresp())
    {
        MQTT311_Print("Successfull pinging!");
    }
    else 
    {
        MQTT311_Print("Unsuccesfull pinging!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*
 * Function: MQTT311_Pingreq
 * ----------------------------
 *   Used to ping the server
 * 
 *   returns: no return value
 */
void MQTT311_Pingreq(void)
{
    
    /* Creating a PINGREQ_MESSAGE structure */
    struct PINGREQ_MESSAGE* pingreq_message_data = (struct PINGREQ_MESSAGE*) pvPortMalloc(sizeof *pingreq_message_data);
    NULL_CHECK(pingreq_message_data)

    /* Filling up the structure - Header Data */
    pingreq_message_data->packet_type = PINGREQ;

    /* Filling up the structure - Rest of the data */
    pingreq_message_data->remaining_length = 0x00;

    /* Connecting a function */
    pingreq_message_data->MQTT311_PingreqWithStruct = &MQTT311_PingreqWithStruct;

    /* Creating a MQTTPacket and adding SUBSCRIBE packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = ePINGREQ;
    mqtt_packet->packet_data.pingreq_message_structure = pingreq_message_data;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY );

}