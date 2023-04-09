/***********************************************************************
* FILENAME:        MQTT311_Connack.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 CONNACK package.
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
 * Function: MQTT311_Connack
 * ----------------------------
 *   Reads back the acknowledgement message from the server.
 *
 *   returns: bool (true if acknowledged)
 */
bool MQTT311_Connack(void) 
{
    /* Creating a connack message */
    struct CONNACK_MESSAGE connack_message_data = {0};

    /* Filling up necessary data */
    connack_message_data.packet_type = CONNACK;
    connack_message_data.remaining_length = CONACK_PACKET_REMAINING_LENGTH;
    connack_message_data.packet_size = CONNACK_PACKET_SIZE;

    while (number_of_bytes_received != atoi(connack_message_data.packet_size))
    {
        MQTT311_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Useful flag for keeping track of success of response message */
    bool success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311_CheckResponseHeader(connack_message_data.packet_type, connack_message_data.remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
        return success_message;
    }

    /* Getting connect acknowledge flags */
    uint8_t connect_acknowledge_flags = bytes_to_receive[2];

    connack_message_data.connect_acknowledge_flags = connect_acknowledge_flags;

    /* Getting connect response code */
    uint8_t connect_response_code = bytes_to_receive[3];

    connack_message_data.connect_response_code = connect_response_code;

    success_message = false;

    /* Checking the connect response code */
    if (connect_response_code == CONNECTION_ACCEPTED)
    {
        MQTT311_Print(CONNECTION_ACCEPTED_MESSAGE);
        success_message = true;
    }
    else if (connect_response_code == UNNACCEPTABLE_PROTOCOL) 
    {
        MQTT311_Print(UNNACCEPTABLE_PROTOCOL_MESSAGE);
    }
    else if (connect_response_code == IDENTIFIER_REJECTED) 
    {
        MQTT311_Print(IDENTIFIER_REJECTED_MESSAGE);
    }
    else if (connect_response_code == SERVER_UNAVAILABLE) 
    {
        MQTT311_Print(SERVER_UNAVAILABLE_MESSAGE);
    }
    else if (connect_response_code == WRONG_USERNAME_OR_PASSWORD) 
    {
        MQTT311_Print(WRONG_USERNAME_OR_PASSWORD_MESSAGE);
    }
    else if (connect_response_code == NOT_AUTHORIZED) 
    {
        MQTT311_Print(NOT_AUTHORIZED_MESSAGE);
    }
    else
    {
        MQTT311_Print(UNSUPPORTED_RESPONSE);
    }

    return success_message;
}