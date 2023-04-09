/***********************************************************************
* FILENAME:        MQTT311_Unsuback.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 UNSUBACK package.
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

/*
 * Function: MQTT311_Unsuback
 * ----------------------------
 *   Reads back the acknowledgement message from the server.
 *
 *   packetIdentifier: identifier of the publish package
 *
 *   returns: bool (true if acknowledged)
 */
bool MQTT311_Unsuback(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct UNSUBACK_MESSAGE unsuback_message_data = {0};

    /* Filling up necessary data */
    unsuback_message_data.packet_type = UNSUBACK;
    unsuback_message_data.remaining_length = UNSUBACK_PACKET_REMAINING_LENGTH;
    unsuback_message_data.packet_size = UNSUBACK_PACKET_SIZE;

    while (number_of_bytes_received != atoi(unsuback_message_data.packet_size))
    {
        MQTT311_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Useful flag for keeping track of sucess of response message */
    bool success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311_CheckResponseHeader(unsuback_message_data.packet_type, unsuback_message_data.remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
        return success_message;
    }

    unsuback_message_data.packet_identifier = MQTT311_GetPacketIdentifier(2);

    /* Checking if this is the correct packet identifier */
    if (unsuback_message_data.packet_identifier != packetIdentifier) 
    {
        success_message = false;
    }

    return success_message;

}