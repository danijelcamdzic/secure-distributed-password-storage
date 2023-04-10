/**
 * @file MQTT311_Pingresp.h
 * @brief Contains definitions, structures, and function declarations for the MQTT 3.1.1 PINGRESP package.
 *
 * This header file contains the definitions, structures, and function declarations required for implementing
 * the MQTT 3.1.1 PINGRESP package. The PINGRESP package is used to respond to a PINGREQ message from an MQTT client.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311_PINGRESP_H
#define MQTT311_PINGRESP_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- PINGRESP MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length(0) |
    ...
*/

/* Packet Information */
#define PINGRESP_PACKET_REMAINING_LENGTH     0x00        /* Packet's fixed remaining length */
#define PINGRESP_PACKET_SIZE                 "2"         /* Packet size */

/* PINGRESP Message Structure */
struct PINGRESP_MESSAGE 
{
    char* packet_size;                              /* Size of the packet */
    uint16_t remaining_length;                      /* Remaining length of the packet  = 0 for PINGRES*/ 
    uint8_t packet_type;                            /* MQTT Message Packet Type*/ 
};

/* Function declarations */
bool MQTT311_Pingresp(void);

#endif /* MQTT311_PINGRESP_H */
