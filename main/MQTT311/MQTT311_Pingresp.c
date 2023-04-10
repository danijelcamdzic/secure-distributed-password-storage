/**
 * @file MQTT311_Pingresp.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PINGRESP package.
 *
 * This file contains the variables and function definitions required for implementing
 * the MQTT 3.1.1 PINGRESP package. The PINGRESP package is used to respond to a PINGREQ message from an MQTT client.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311/MQTT311.h"

/**
 * @brief Reads back the ping response message from the MQTT broker.
 *
 * This function is used to read back the PINGRESP message from the MQTT broker, indicating that the broker has
 * received and processed a PINGREQ message sent by the client. The function returns true if the response is received
 * successfully, and false otherwise.
 *
 * @return A boolean value indicating whether the PINGRESP message was received successfully (true) or not (false).
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
