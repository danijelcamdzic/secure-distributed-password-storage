/***********************************************************************
* FILENAME:        MQTT311_Puback.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 PUBACK package.
*
* NOTES:
*       
*
* AUTHOR:          Danijel Camdzic     
*
*   
* DATE:            19 Aug 21
*
*
* CHANGES:
*
* VERSION:         DATE:          WHO:         DETAIL:
* 0.00.0           19 Aug 21      DC           Initial state of the file
*
*/

#ifndef MQTT311_PUBACK_H
#define MQTT311_PUBACK_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- PUBACK MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |

*/

/* Packet Information */
#define PUBACK_PACKET_REMAINING_LENGTH  0x02        /* Packet's fixed remaining length */
#define PUBACK_PACKET_SIZE              "4"         /* Packet size */

/* PUBACK Message Structure */
struct PUBACK_MESSAGE 
{
    char* packet_size;

    uint16_t remaining_length;                      /* Remaining length of the packet == 2 in puback package*/  
    uint16_t packet_identifier;                     /* Packet identifier from the publish message */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
bool MQTT311_Puback(uint16_t packetIdentifier);

#endif