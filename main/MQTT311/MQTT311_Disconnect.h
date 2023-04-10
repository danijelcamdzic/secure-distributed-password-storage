/**
 * @file MQTT311_Disconnect.h
 * @brief Contains definitions, structures, and function declarations for the MQTT 3.1.1 DISCONNECT package.
 *
 * This header file contains the definitions, structures, and function declarations required for implementing
 * the MQTT 3.1.1 DISCONNECT package. The DISCONNECT package is used to gracefully terminate an MQTT connection.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311_DISCONNECT_H
#define MQTT311_DISCONNECT_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- DISCONNECT MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length(0) |
    ...
*/

/* DISCONNECT Message Structure */
struct DISCONNECT_MESSAGE 
{
    void (*MQTT311_DisconnectWithStruct)(struct DISCONNECT_MESSAGE *);
    
    uint16_t remaining_length;                      /* Remaining length of the packet  = 0 for PINGRES*/  
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* ----------------------------------------------------------------------------------------- */

/* Function declarations */
void MQTT311_Disconnect(void);

#endif /* MQTT311_DISCONNECT_H */