/**
 * @file MQTT311Client_Puback.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PUBACK package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Puback.h"

/**
 * @brief Reads back the acknowledgement message from the server.
 *
 * @param packetIdentifier Identifier of the publish package
 *
 * @return bool True if acknowledged
 */
bool MQTT311Client_Puback(uint16_t packetIdentifier) 
{

    /* Creating a connack message */
    struct PUBACK_MESSAGE puback_message_data = {0};

    /* Filling up necessary data */
    puback_message_data.packet_type = PUBACK;
    puback_message_data.remaining_length = PUBACK_PACKET_REMAINING_LENGTH;
    puback_message_data.packet_size = PUBACK_PACKET_SIZE;

    while (MQTT311_RECEIVED_BYTES != atoi(puback_message_data.packet_size))
    {
        MQTT311Client_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    MQTT311_RECEIVED_BYTES = 0;

    /* Get remaining packet parts */
    bool success_message = MQTT311Client_GetPubPacketInfo(packetIdentifier, puback_message_data.packet_type, puback_message_data.remaining_length, 0);

    return success_message;
}


