/**
 * @file MQTT311Client_Unsuback.h
 * @brief Contains definitions, structure and function declarations
 * for the MQTT 3.1.1 UNSUBACK package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_UNSUBACK_H
#define MQTT311Client_UNSUBACK_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "MQTT311Client/MQTT311Client.h"

/*   ----------------- UNSUBACK MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |
    ...
*/

/* Packet Information */
#define UNSUBACK_PACKET_REMAINING_LENGTH     0x02        /**< Packet's fixed remaining length */
#define UNSUBACK_PACKET_SIZE                 "4"         /**< Packet size */

/* UNSUBACK Message Structure */
struct UNSUBACK_MESSAGE 
{
    char* packet_size;                              /**< Packet size = 2 for unsuback */

    uint16_t remaining_length;                      /**< Remaining length of the packet */  
    uint16_t packet_identifier;                     /**< Packet identifier from the publish message */
    uint8_t packet_type;                            /**< MQTT Message Packet Type*/
};

/* Function declarations */
bool MQTT311Client_Unsuback(uint16_t packetIdentifier);

#endif /* MQTT311Client_UNSUBACK_H */

