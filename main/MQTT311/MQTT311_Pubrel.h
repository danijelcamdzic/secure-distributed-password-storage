/**
 * @file MQTT311_Pubrel.h
 * @brief Contains definitions, structure and function declarations
 * for the MQTT 3.1.1 PUBREL package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311_PUBREL_H
#define MQTT311_PUBREL_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- PUBREL MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type + Reserved(0x02) | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |

*/

#define PUBREL_RESERVED                      0x02    /* Reserved field of the header, must be 0x02 */

/* Packet Information */
#define PUBREL_PACKET_REMAINING_LENGTH  0x02        /* Packet's fixed remaining length */
#define PUBREL_PACKET_SIZE              "4"         /* Packet size */

/* PUBREC Message Structure */
struct PUBREL_MESSAGE 
{
    char* packet_size;

    uint16_t remaining_length;                      /* Remaining length of the packet == 2 in pubrel package*/  
    uint16_t packet_identifier;                     /* Packet identifier from the publish message */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
bool MQTT311_Pubrel(uint16_t packetIdentifier);

#endif /* MQTT311_PUBREL_H */

