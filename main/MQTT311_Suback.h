/***********************************************************************
* FILENAME:        MQTT311_Suback.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 SUBACK package.
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

#ifndef MQTT311_SUBACK_H
#define MQTT311_SUBACK_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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
bool MQTT311_Suback(uint16_t packetIdentifier);

#endif