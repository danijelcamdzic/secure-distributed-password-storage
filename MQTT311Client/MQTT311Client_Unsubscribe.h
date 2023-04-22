/**
 * @file MQTT311Client_Unsubscribe.h
 * @brief Contains definitions, structure and function declarations for the MQTT 3.1.1 UNSUBSCRIBE package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_UNSUBSCRIBE_H
#define MQTT311Client_UNSUBSCRIBE_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   ----------------- UNSUBSCRIBE MESSAGE STRUCTURE | B0 | B1 | B2 | .. ----------------------

     | MQTT Control Packet Type + Reserved(0x02) | Remaining Length | Packet Identifier MSB | Packet Identifier LSB |...
     | Topic Name Length MSB | Topic Name Length LSB | Topic Name | ...
     | Topic Name Length MSB | Topic Name Length LSB | Topic Name | ...
    ...
*/

#define UNSUB_RESERVED                      0x02    /* Reserved field of the header */

/* UNSUBSCRIBE Message Structure */
struct UNSUBSCRIBE_MESSAGE 
{
    void (*MQTT311Client_UnsubscribeWithStruct)(struct UNSUBSCRIBE_MESSAGE *);

    char* topic_name;                               /* Topic name to unsubscribe from */
    
    uint16_t remaining_length;                      /* Remaining length of the packet */  
    uint16_t packet_identifier;                     /* Packet identifier from the publish message */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
};

/* Function declarations */
void MQTT311Client_Unsubscribe(uint16_t packet_id, const char* topicName);

#endif /* MQTT311Client_UNSUBSCRIBE_H */