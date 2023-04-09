/***********************************************************************
* FILENAME:        MQTT311_Connack.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  for the MQTT 3.1.1 CONNACK package.
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

#ifndef MQTT311_CONNACK_H
#define MQTT311_CONNACK_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- CONNACK MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Connect Acknowledge Flags | Connect Return Codes |

*/

/* Connect acknowledge flags */
#define SP                              0           /* Session present - if Clean Session == 1, server must set SP = 0 ..*/
                                                    /* .. and Connect Return Code = 0x00 */

/* Connect Return codes */
#define CONNECTION_ACCEPTED             0x00        /* Connection accepted */
#define UNNACCEPTABLE_PROTOCOL          0x01        /* The Server does not support the level of the MQTT protocol requested by the Client */
#define IDENTIFIER_REJECTED             0x02        /* The Client identifier is correct UTF-8 but not allowed by the Server */
#define SERVER_UNAVAILABLE              0x03        /* The Network Connection has been made but the MQTT service is unavailable */
#define WRONG_USERNAME_OR_PASSWORD      0x04        /* The data in the user name or password is malformed */
#define NOT_AUTHORIZED                  0x05        /* The Client is not authorized to connect */

/* Packet Information */
#define CONACK_PACKET_REMAINING_LENGTH  0x02        /* Packet's fixed remaining length */
#define CONNACK_PACKET_SIZE             "4"         /* Packet size */

/* Text macros */
#define CONNECTION_ACCEPTED_MESSAGE               "Connection Accepted"    
#define UNNACCEPTABLE_PROTOCOL_MESSAGE            "Connection Refused, unacceptable protocol version"
#define IDENTIFIER_REJECTED_MESSAGE               "Connection Refused, identifier rejected"  
#define SERVER_UNAVAILABLE_MESSAGE                "Connection Refused, Server unavailable"
#define WRONG_USERNAME_OR_PASSWORD_MESSAGE        "Connection Refused, bad user name or password"
#define NOT_AUTHORIZED_MESSAGE                    "Connection Refused, not authorized"      
#define UNSUPPORTED_RESPONSE                      "Unsupported response"  
#define SUB_MAX_0                                 "Subscribe success - Maximum QoS 0"
#define SUB_MAX_1                                 "Subscribe success - Maximum QoS 1"
#define SUB_MAX_2                                 "Subscribe success - Maximum QoS 2"
#define SUB_FAILURE                               "Failure to subscribe!"

/* Connack Message Structure */
struct CONNACK_MESSAGE 
{
    char* packet_size;
    
    uint16_t remaining_length;

    uint8_t packet_type;
    uint8_t connect_acknowledge_flags;
    uint8_t connect_response_code;
};

/* Function declarations */
bool MQTT311_Connack(void);
#endif