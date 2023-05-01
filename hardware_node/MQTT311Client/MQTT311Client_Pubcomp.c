/**
 * @file MQTT311Client_Pubcomp.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PUBCOMP package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Pubcomp.h"

/**
 * @brief Reads back the acknowledgement message from the server.
 *
 * @param packetIdentifier Identifier of the publish package
 *
 * @return bool True if acknowledged
 */
bool MQTT311Client_Pubcomp(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct PUBCOMP_MESSAGE pubcomp_message_data = {0};

    /* Filling up necessary data */
    pubcomp_message_data.packet_type = PUBCOMP;
    pubcomp_message_data.remaining_length = PUBCOMP_PACKET_REMAINING_LENGTH;
    pubcomp_message_data.packet_size = PUBCOMP_PACKET_SIZE;

    while (MQTT311_RECEIVED_BYTES != atoi(pubcomp_message_data.packet_size))
    {
        MQTT311Client_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    MQTT311_RECEIVED_BYTES = 0;

    /* Get remaining packet parts */
    bool success_message = MQTT311Client_GetPubPacketInfo(packetIdentifier, pubcomp_message_data.packet_type, pubcomp_message_data.remaining_length, 0);

    return success_message;
}

