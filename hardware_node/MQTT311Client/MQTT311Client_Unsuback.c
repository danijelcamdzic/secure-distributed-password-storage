/**
 * @file MQTT311Client_Unsuback.c
 * @brief Contains variables and function definitions for the 
 * MQTT 3.1.1 UNSUBACK package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client.h"

/**
 * @brief Reads back the acknowledgement message from the server.
 *
 * @param packetIdentifier Identifier of the publish package
 *
 * @return bool True if acknowledged
 */
bool MQTT311Client_Unsuback(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct UNSUBACK_MESSAGE unsuback_message_data = {0};

    /* Filling up necessary data */
    unsuback_message_data.packet_type = UNSUBACK;
    unsuback_message_data.remaining_length = UNSUBACK_PACKET_REMAINING_LENGTH;
    unsuback_message_data.packet_size = UNSUBACK_PACKET_SIZE;

    while (MQTT311_RECEIVED_BYTES != atoi(unsuback_message_data.packet_size))
    {
        MQTT311Client_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    MQTT311_RECEIVED_BYTES = 0;

    /* Useful flag for keeping track of sucess of response message */
    bool success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311Client_CheckResponseHeader(unsuback_message_data.packet_type, unsuback_message_data.remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
        return success_message;
    }

    unsuback_message_data.packet_identifier = MQTT311Client_GetPacketIdentifier(2);

    /* Checking if this is the correct packet identifier */
    if (unsuback_message_data.packet_identifier != packetIdentifier) 
    {
        success_message = false;
    }

    return success_message;
}

