/***********************************************************************
* FILENAME:        MQTT311_Pingreq.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 PINGREQ package.
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

#ifndef MQTT311_PINGREQ_H
#define MQTT311_PINGREQ_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- PINGREQ MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length(0) |
    ...
*/

/* PINGREQ Message Structure */
struct PINGREQ_MESSAGE 
{
    void (*MQTT311_PingreqWithStruct)(struct PINGREQ_MESSAGE*);
    uint16_t remaining_length;                      /* Remaining length of the packet  = 0 for PINGREQ*/  
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
void MQTT311_Pingreq(void);

#endif