/***********************************************************************
* FILENAME:        MQTT311_Suback.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 SUBACK package.
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

/*
 * Function: MQTT311_Suback
 * ----------------------------
 *   Reads back the acknowledgement message from the server.
 *
 *   packetIdentifier: identifier of the publish package
 *
 *   returns: bool (true if acknowledged)
 */
bool MQTT311_Suback(uint16_t packetIdentifier) 
{
    /* Creating a connack message */
    struct SUBACK_MESSAGE suback_message_data = {0};

    /* Filling up necessary data */
    suback_message_data.packet_type = SUBACK;
    suback_message_data.remaining_length = SUBACK_PACKET_REMAINING_LENGTH;
    suback_message_data.packet_size = SUBACK_PACKET_SIZE;

    while (number_of_bytes_received != atoi(suback_message_data.packet_size))
    {
        MQTT311_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Useful flag for keeping track of sucess of response message */
    bool success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311_CheckResponseHeader(suback_message_data.packet_type, suback_message_data.remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
        return success_message;
    }

    suback_message_data.packet_identifier = MQTT311_GetPacketIdentifier(2);

    /* Checking if this is the correct packet identifier */
    if (suback_message_data.packet_identifier != packetIdentifier) 
    {
        success_message = false;
        return success_message;
    }

    /* Getting connect response code */
    uint8_t suback_return_codes = bytes_to_receive[4];

    suback_message_data.return_codes = suback_return_codes;

    /* Checking the connect response code */
    if (suback_return_codes & RETURN_CODE_BIT_7)
    {
        MQTT311_Print(SUB_FAILURE);
        success_message = false;
    }
    else if (suback_return_codes & RETURN_CODE_BIT_0) 
    {
        MQTT311_Print(SUB_MAX_1);
    }
    else if (suback_return_codes & RETURN_CODE_BIT_1) 
    {
        MQTT311_Print(SUB_MAX_2);
    }
    else 
    {
        MQTT311_Print(SUB_MAX_0);
    }

    return success_message;

}