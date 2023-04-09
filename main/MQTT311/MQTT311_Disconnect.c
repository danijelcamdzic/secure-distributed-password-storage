/***********************************************************************
* FILENAME:        MQTT311_Disconnect.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 DISCONNECT package.
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
static void MQTT311_DisconnectWithStruct(struct DISCONNECT_MESSAGE *disconnect_message_data);

/*
 * Function: MQTT311_DisconnectWithStruct
 * ----------------------------
 *   Disconnects from the server.
 *
 *   disconnect_message_data: disconnect message structure
 *
 *   returns: no return value
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

/*
 * Function: MQTT311_Disconnect
 * ----------------------------
 *   Disconnect from the server
 *
 * 
 *   returns: no return value
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