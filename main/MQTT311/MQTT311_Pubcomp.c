/***********************************************************************
* FILENAME:        MQTT311_Pubcomp.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 PUBCOMP package.
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
 * Function: MQTT311_Pubcomp
 * ----------------------------
 *   Reads back the acknowledgement message from the server.
 *
 *   packetIdentifier: identifier of the publish package
 *
 *   returns: bool (true if acknowledged)
 */
bool MQTT311_Pubcomp(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct PUBCOMP_MESSAGE pubcomp_message_data = {0};

    /* Filling up necessary data */
    pubcomp_message_data.packet_type = PUBCOMP;
    pubcomp_message_data.remaining_length = PUBCOMP_PACKET_REMAINING_LENGTH;
    pubcomp_message_data.packet_size = PUBCOMP_PACKET_SIZE;

    while (number_of_bytes_received != atoi(pubcomp_message_data.packet_size))
    {
        MQTT311_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Get remaining packet parts */
    bool success_message = MQTT311_GetPubPacketInfo(packetIdentifier, pubcomp_message_data.packet_type, pubcomp_message_data.remaining_length, 0);

    return success_message;
}