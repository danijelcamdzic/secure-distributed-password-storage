/***********************************************************************
* FILENAME:        MQTT311_Pubrec.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 PUBREC package.
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

#ifndef MQTT311_PUBREC_H
#define MQTT311_PUBREC_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- PUBREC MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |

*/

/* Packet Information */
#define PUBREC_PACKET_REMAINING_LENGTH  0x02        /* Packet's fixed remaining length */
#define PUBREC_PACKET_SIZE              "4"         /* Packet size */

/* PUBREC Message Structure */
struct PUBREC_MESSAGE 
{
    char* packet_size;

    uint16_t remaining_length;                      /* Remaining length of the packet == 2 in pubrec package*/  
    uint16_t packet_identifier;                     /* Packet identifier from the publish message */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
bool MQTT311_Pubrec(uint16_t packetIdentifier);

#endif