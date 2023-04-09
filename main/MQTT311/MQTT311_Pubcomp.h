/***********************************************************************
* FILENAME:        MQTT311_Pubcomp.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 PUBCOMP package.
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

#ifndef MQTT311_PUBCOMP_H
#define MQTT311_PUBCOMP_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- PUBCOMP MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |

*/

/* Packet Information */
#define PUBCOMP_PACKET_REMAINING_LENGTH  0x02        /* Packet's fixed remaining length */
#define PUBCOMP_PACKET_SIZE              "4"         /* Packet size */

/* PUBCOMP Message Structure */
struct PUBCOMP_MESSAGE 
{
    char* packet_size;
    uint16_t remaining_length;                      /* Remaining length of the packet == 2 in pubcomp package*/  
    uint16_t packet_identifier;                     /* Packet identifier from the publish message */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
bool MQTT311_Pubcomp(uint16_t packetIdentifier);

#endif