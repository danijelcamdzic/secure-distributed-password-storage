/***********************************************************************
* FILENAME:        MQTT311_Pingresp.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 PINGRESP package.
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
 * Function: get_pingresp
 * ----------------------------
 *   Reads back the ping response message from the server.
 *
 *   returns: bool (true if acknowledged)
 */
bool MQTT311_Pingresp(void) 
{

    /* Creating a connack message */
    struct PINGRESP_MESSAGE pingresp_message_data = {0};

    /* Filling up necessary data */
    pingresp_message_data.packet_type = PINGRESP;
    pingresp_message_data.remaining_length = PINGRESP_PACKET_REMAINING_LENGTH;
    pingresp_message_data.packet_size = PINGRESP_PACKET_SIZE;

    while (number_of_bytes_received != atoi(pingresp_message_data.packet_size))
    {
        MQTT311_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Useful flag for keeping track of sucess of response message */
    bool success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311_CheckResponseHeader(pingresp_message_data.packet_type, pingresp_message_data.remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
    }

    return success_message;
}