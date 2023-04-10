/**
 * @file MQTT311_Pubrel.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PUBREL package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311/MQTT311.h"

/**
 * @brief Reads back the acknowledgement message from the server.
 *
 * @param packetIdentifier Identifier of the publish package
 *
 * @return bool True if acknowledged
 */
bool MQTT311_Pubrel(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct PUBREL_MESSAGE pubrel_message_data = {0};

    /* Filling up necessary data */
    pubrel_message_data.packet_type = PUBREL | PUBREL_RESERVED;
    pubrel_message_data.remaining_length = PUBREL_PACKET_REMAINING_LENGTH;
    pubrel_message_data.packet_size = PUBREL_PACKET_SIZE;

    while (number_of_bytes_received != atoi(pubrel_message_data.packet_size))
    {
        MQTT311_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Get remaining packet parts */
    bool success_message = MQTT311_GetPubPacketInfo(packetIdentifier, pubrel_message_data.packet_type, pubrel_message_data.remaining_length, 0);

    return success_message;
}

