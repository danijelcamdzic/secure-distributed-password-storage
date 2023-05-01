/**
 * @file MQTT311Client_Pubrel.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PUBREL package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Pubrel.h"

/**
 * @brief Reads back the acknowledgement message from the server.
 *
 * @param packetIdentifier Identifier of the publish package
 *
 * @return bool True if acknowledged
 */
bool MQTT311Client_Pubrel(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct PUBREL_MESSAGE pubrel_message_data = {0};

    /* Filling up necessary data */
    pubrel_message_data.packet_type = PUBREL | PUBREL_RESERVED;
    pubrel_message_data.remaining_length = PUBREL_PACKET_REMAINING_LENGTH;
    pubrel_message_data.packet_size = PUBREL_PACKET_SIZE;

    while (MQTT311_RECEIVED_BYTES != atoi(pubrel_message_data.packet_size))
    {
        MQTT311Client_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    MQTT311_RECEIVED_BYTES = 0;

    /* Get remaining packet parts */
    bool success_message = MQTT311Client_GetPubPacketInfo(packetIdentifier, pubrel_message_data.packet_type, pubrel_message_data.remaining_length, 0);

    return success_message;
}

