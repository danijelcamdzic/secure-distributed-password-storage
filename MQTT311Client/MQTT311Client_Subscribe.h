/**
 * @file MQTT311Client_Subscribe.h
 * @brief Contains definitions, structure and function declarations
 * for the MQTT 3.1.1 SUBSCRIBE package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_SUBSCRIBE_H
#define MQTT311Client_SUBSCRIBE_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- SUBSCRIBE MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type + Reserved (0x02) | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |...
     | Topic Name Length MSB | Topic Name Length LSB | Requested QoS | ...
     | Topic Name Length MSB | Topic Name Length LSB | Requested QoS | ...
     | Topic Name Length MSB | Topic Name Length LSB | Requested QoS | ...
     ...

*/

#define SUB_RESERVED                        0x02    /* Reserved field of the header, must be 0x02 */

#define SUB_QoS1                            0       /* QoS Level */
#define SUB_QoS2                            1       /* QoS Level */

/* Return Type of the Subscribe Message */
typedef enum SubscribeMessageResult_e
{
    SUBSCRIBE_SUCCESS = 0,
    SUBSCRIBE_FAIL
} SubscribeMessageResult_t;

/* SUBSCRIBE Message Structure */
struct SUBSCRIBE_MESSAGE 
{
    SubscribeMessageResult_t (*MQTT311Client_SubscribeWithStruct)(struct SUBSCRIBE_MESSAGE *);

    char* topic_name;                               /* Topic Name */

    uint16_t remaining_length;                      /* Remaining length of the packet */
    uint16_t packet_identifier;                     /* Packet identifier */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/

    uint8_t requested_qos;                          /* Requested QoS */
};

/* Function declarations */
void MQTT311Client_Subscribe(uint16_t packet_id, const char* topicName, uint8_t requested_qos);

#endif /* MQTT311Client_SUBSCRIBE_H */

