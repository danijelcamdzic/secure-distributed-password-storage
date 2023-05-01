/**
 * @file MQTT311Client_Pingreq.h
 * @brief Contains definitions, structures, and function declarations for the MQTT 3.1.1 PINGREQ package.
 *
 * This header file contains the definitions, structures, and function declarations required for implementing
 * the MQTT 3.1.1 PINGREQ package. The PINGREQ package is used to keep the MQTT connection alive by periodically
 * sending a ping request to the broker.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_PINGREQ_H
#define MQTT311Client_PINGREQ_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "MQTT311Client/MQTT311Client.h"

/*   ----------------- PINGREQ MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length(0) |
    ...
*/

/* Return Type of the Pingreq Message */
typedef enum PingreqMessageResult_e
{
    PINGREQ_SUCCESS = 0,
    PINGREQ_FAIL
} PingreqMessageResult_t;

/* PINGREQ Message Structure */
struct PINGREQ_MESSAGE 
{
    PingreqMessageResult_t (*MQTT311Client_PingreqWithStruct)(struct PINGREQ_MESSAGE*);
    uint16_t remaining_length;                      /**< Remaining length of the packet  = 0 for PINGREQ*/  
    uint8_t packet_type;                            /**< MQTT Message Packet Type*/
};

/* ----------------------------------------------------------------------------------------- */

/* Function declarations */
void MQTT311Client_Pingreq(void);

#endif /* MQTT311Client_PINGREQ_H */
