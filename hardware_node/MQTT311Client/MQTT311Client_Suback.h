/**
 * @file MQTT311Client_Suback.h
 * @brief Contains definitions, structure and function declarations
 * for the MQTT 3.1.1 SUBACK package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_SUBACK_H
#define MQTT311Client_SUBACK_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "MQTT311Client/MQTT311Client.h"

/*   ----------------- SUBACK MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |

*/

/* Return codes bits */
#define RETURN_CODE_BIT_7                   0x80    /* Indicates success (0) or failure (1) */
#define RETURN_CODE_BIT_1                   0x02    /* Indicate the Maximum QoS Level*/
#define RETURN_CODE_BIT_0                   0x01        /* "00" - Success, Maximum QoS 0 */
                                                        /* "01" - Success, Maximum QoS 1 */
                                                        /* "02" - Success, Maximum QoS 2 */

/* Packet Information */
#define SUBACK_PACKET_REMAINING_LENGTH      0x03        /* Packet's fixed remaining length */
#define SUBACK_PACKET_SIZE                  "5"         /* Packet size */

/* Text macros */
#define SUB_MAX_0                                 "Subscribe success - Maximum QoS 0"
#define SUB_MAX_1                                 "Subscribe success - Maximum QoS 1"
#define SUB_MAX_2                                 "Subscribe success - Maximum QoS 2"
#define SUB_FAILURE                               "Failure to subscribe!"

/* SUBACK Message Structure */
struct SUBACK_MESSAGE 
{
    char* packet_size;
    
    uint16_t remaining_length;                      /* Remaining length of the packet == 3 in suback package*/  
    uint16_t packet_identifier;                     /* Packet identifier from the publish message */
    uint8_t return_codes;                           /* Return code sent by the server */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
bool MQTT311Client_Suback(uint16_t packetIdentifier);

#endif /* MQTT311Client_SUBACK_H */

