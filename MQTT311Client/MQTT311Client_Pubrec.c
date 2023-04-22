/**
 * @file MQTT311Client_Pubrec.c
 * @brief Contains variables and function definitions for the MQTT 3.1.1 PUBREC package.
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
bool MQTT311Client_Pubrec(uint16_t packetIdentifier) 
{
    /* Creating a connack message */
    struct PUBREC_MESSAGE pubrec_message_data = {0};

    /* Filling up necessary data */
    pubrec_message_data.packet_type = PUBREC;
    pubrec_message_data.remaining_length = PUBREC_PACKET_REMAINING_LENGTH;
    pubrec_message_data.packet_size = PUBREC_PACKET_SIZE;

    while (number_of_bytes_received != atoi(pubrec_message_data.packet_size))
    {
        MQTT311Client_ReceiveFromMQTTBroker();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    number_of_bytes_received = 0;

    /* Get remaining packet parts */
    bool success_message = MQTT311Client_GetPubPacketInfo(packetIdentifier, pubrec_message_data.packet_type, pubrec_message_data.remaining_length, 0);

    return success_message;
}

