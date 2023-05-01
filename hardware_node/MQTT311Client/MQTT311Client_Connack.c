/**
 * @file MQTT311Client_Connack.c
 * @brief Contains variable and function definitions for the MQTT 3.1.1 CONNACK package.
 *
 * This file contains the variable and function definitions required for implementing the MQTT 3.1.1 CONNACK package.
 * The CONNACK package is used to acknowledge receipt and acceptance of a connection request from an MQTT client to an
 * MQTT broker.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Connack.h"

/**
 * @brief Reads the acknowledgement message from the MQTT broker.
 *
 * This function reads the acknowledgement message from the MQTT broker after a connection request has been made.
 * It returns a boolean value indicating whether the connection was acknowledged (true) or not (false).
 *
 * @return bool True if the connection was acknowledged, false otherwise.
 */
bool MQTT311Client_Connack(void) 
{
    /* Creating a connack message */
    struct CONNACK_MESSAGE connack_message_data = {0};

    /* Filling up necessary data */
    connack_message_data.packet_type = CONNACK;
    connack_message_data.remaining_length = CONACK_PACKET_REMAINING_LENGTH;
    connack_message_data.packet_size = CONNACK_PACKET_SIZE;

    while (MQTT311_RECEIVED_BYTES != atoi(connack_message_data.packet_size))
    {
        MQTT311Client_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    MQTT311_RECEIVED_BYTES = 0;

    /* Useful flag for keeping track of success of response message */
    bool success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311Client_CheckResponseHeader(connack_message_data.packet_type, connack_message_data.remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
        return success_message;
    }

    /* Getting connect acknowledge flags */
    uint8_t connect_acknowledge_flags = MQTT311_RECEIVE_BUFFER[2];

    connack_message_data.connect_acknowledge_flags = connect_acknowledge_flags;

    /* Getting connect response code */
    uint8_t connect_response_code = MQTT311_RECEIVE_BUFFER[3];

    connack_message_data.connect_response_code = connect_response_code;

    success_message = false;

    /* Checking the connect response code */
    if (connect_response_code == CONNECTION_ACCEPTED)
    {
        MQTT311Client_Print(CONNECTION_ACCEPTED_MESSAGE);
        success_message = true;
    }
    else if (connect_response_code == UNNACCEPTABLE_PROTOCOL) 
    {
        MQTT311Client_Print(UNNACCEPTABLE_PROTOCOL_MESSAGE);
    }
    else if (connect_response_code == IDENTIFIER_REJECTED) 
    {
        MQTT311Client_Print(IDENTIFIER_REJECTED_MESSAGE);
    }
    else if (connect_response_code == SERVER_UNAVAILABLE) 
    {
        MQTT311Client_Print(SERVER_UNAVAILABLE_MESSAGE);
    }
    else if (connect_response_code == WRONG_USERNAME_OR_PASSWORD) 
    {
        MQTT311Client_Print(WRONG_USERNAME_OR_PASSWORD_MESSAGE);
    }
    else if (connect_response_code == NOT_AUTHORIZED) 
    {
        MQTT311Client_Print(NOT_AUTHORIZED_MESSAGE);
    }
    else
    {
        MQTT311Client_Print(UNSUPPORTED_RESPONSE);
    }

    return success_message;
}
