/**
 * @file MQTT311Client_Publish.h
 * @brief Contains definitions, structure and function declarations
 * for the MQTT 3.1.1 PUBLISH package.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_PUBLISH_H
#define MQTT311Client_PUBLISH_H

/* Included libraries */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*   -------------------- PUBLISH MESSAGE STRUCTURE | B0 | B1 | B2 | .. ---------------------------------

     | MQTT Control Packet Type + DUP Flag + QoS Level + Retain | Remaining Length |...
     | Topic Name Length MSB | Topic Name Length LSB | Topic Name | Packet Identifier if QoS = 1 or QoS = 2 | ...
     | Payload .. |

*/

/* Header flags */
#define DUP_FLAG                        3           /* Specifies delivery or redelivery of publish packet */
#define QOS_LEVEL2                      2           /* Specifies QoS level */
#define QOS_LEVEL1                      1           /* Specifies QoS level */
                                                    /* QoS Levels */
                                                    /*
                                                        "00" - At most once delivery
                                                        "01" - At least once delivery
                                                        "10" - Exactly once delivery
                                                        "11" - Reserved - must not be used
                                                    */
#define RETAIN                          0  

/* Return Type of the Publish Message */
typedef enum PublishMessageResult_e
{
    PUBLISH_SUCCESS = 0,
    PUBLISH_FAIL
} PublishMessageResult_t;

/* Publish Message Structure */
struct PUBLISH_MESSAGE 
{
    /* Function to which it is sent */
    PublishMessageResult_t (*MQTT311Client_PublishWithStruct)(struct PUBLISH_MESSAGE *);

    char* topicName;                                /* Name of the message topic */
    char* payload;                                  /* Data of the message */

    uint16_t remaining_length;                      /* Remaining length of the packet */   
    uint16_t packetIdentifier;                      /* Packet identifier present only if QoS == 1 or QoS == 2 */
    uint8_t packet_type;                            /* MQTT Message Packet Type*/
    
    /* Fixed header - possible configuration */
    bool dup;                                       /* Delivery or re-delivery of packet */
    bool qos1;                                      /* Assurance of delivery */
    bool qos2;                                      /* Assurance of delivery */
    bool retain;                                    /* Server MUST store the Application Message and it's QoS */                  
};

/* ----------------------------------------------------------------------------------------- */

/* Function declarations */
void MQTT311Client_Publish(uint8_t header_flags, const char* topicName, uint16_t packetIdentifier, const char* payload);

#endif /* MQTT311Client_PUBLISH_H */

