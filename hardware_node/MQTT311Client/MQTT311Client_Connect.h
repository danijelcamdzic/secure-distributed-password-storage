/**
 * @file MQTT311Client_Connect.h
 * @brief Contains definitions, structures, and function declarations for the MQTT 3.1.1 CONNECT package.
 *
 * This header file contains the definitions, structures, and function declarations required for implementing
 * the MQTT 3.1.1 CONNECT package. The CONNECT package is used to establish a connection between an MQTT client
 * and an MQTT broker.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_CONNECT_H
#define MQTT311Client_CONNECT_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "MQTT311Client/MQTT311Client.h"

/*   ----------------- CONNECT MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type | Remaining Length | Protocol Name Length MSB (0) | ...
 ... | Protocol Name Length LSB (4) | M | Q | T | T | Protocol Level (4) | ...
 ... | Connect Flags | Keep Alive MSB | Keep Alive LSB | Payload -> (L) -> Client Identifier ...
       -> (L) -> Will Topic -> (L) -> Will Message -> (L) -> Username -> (L)  -> Password |... 

*/

/* Protocol Name+VERSION Length */
#define PROTOCOL_NAME_MSB               0x00
#define PROTOCOL_NAME_LSB               0x04     

/* Protocol Name */
#define PROTOCOL_NAME                   "MQTT"

/* Protocol Version*/
#define PROTOCOL_VERSION                0x04        /* Protocol level for MQTT 3.1.1 is 4 */

/* Connect Flags */
#define CONNECT_FLAGS                   0x00
#define USERNAME_FLAG                   7           /* Specifies whether username is present in the payload */
#define PASSWORD_FLAG                   6           /* Specifies whether password is present in the payload */
#define WILL_RETAIN_FLAG                5           /* Specifies if Will Message is to be retained when published */
#define WILL_QoS2_FLAG                  4           /* Specifies the QoS level used when publishing Will Message */
#define WILL_QoS1_FLAG                  3           /* Specifies the QoS level used when publishing Will Message */
#define WILL_FLAG                       2           /* Specifies whether Will Message is stored on the server*/
#define CLEAN_SESSION_FLAG              1           /* Specifies the handling of the Session state */
#define RESERVED                        0

/* Return Type of the Connect Message */
typedef enum ConnectMessageResult_e
{
    CONNECT_SUCCESS = 0,
    CONNECT_FAIL
} ConnectMessageResult_t;

/* Connect Message Structure */
struct CONNECT_MESSAGE 
{
    /* Function to which it is sent */
    ConnectMessageResult_t (*MQTT311Client_ConnectWithStruct)(struct CONNECT_MESSAGE *);

    char* willTopic;
    char* willMessage;
    char* protocol_name;

    uint16_t keep_alive;  
    uint16_t remaining_length;                      /* Remaining length of the packet */
    uint16_t protocol_name_length;

    uint8_t packet_type;                            /* MQTT Message Packet Type*/
    uint8_t protocol_version;

    /* connect flags */
    bool _username;                                 /* Username present in payload? */
    bool _password;                                 /* Password present in payload? */
    bool will_retain;                               /* Will retain present in payload? */
    bool will_qos1;                                 /* will QoS1 present in payload? */
    bool will_qos2;                                 /* Will QoS2 present in payload? */
    bool clean_session;                             /* Clean Session present in payload? */
    bool reserved;                                
};

/* ----------------------------------------------------------------------------------------- */

/* Function declarations */
void MQTT311Client_Connect(uint8_t connect_flags, uint16_t keepalive, const char* will_topic, const char* will_message);

#endif /* MQTT311Client_CONNECT_H */
