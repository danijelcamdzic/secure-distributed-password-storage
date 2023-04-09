/***********************************************************************
* FILENAME:        MQTT311_Pingresp.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 PINGRESP package.
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

#endif