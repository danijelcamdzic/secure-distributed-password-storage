/**
 * @file MQTT311_Utilities.h
 * @brief Contains definitions, structures, and function declarations used by other MQTT 3.1.1 packets.
 * 
 * This file contains various definitions, structures, and function declarations used by other
 * MQTT 3.1.1 packets in the implementation. 
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311_UTILITIES_H
#define MQTT311_UTILITIES_H

#include "MQTT311/MQTT311.h"

/* External Function */
typedef void (*MQTT311_ConnectTCPSocketPtr)(const char*, uint16_t);
typedef void (*MQTT311_CloseTCPSocketPtr)(void);
typedef void (*MQTT311_SendToTCPSocketPtr)(const char*, uint16_t);
typedef void (*MQTT311_ReadFromTCPSocketPtr)(char*, uint16_t*);
typedef void (*MQTT311_PrintPtr)(char*);

extern MQTT311_ConnectTCPSocketPtr MQTT311_ConnectTCPSocket;
extern MQTT311_CloseTCPSocketPtr MQTT311_CloseTCPSocket;
extern MQTT311_SendToTCPSocketPtr MQTT311_SendToTCPSocket;
extern MQTT311_ReadFromTCPSocketPtr MQTT311_ReadFromTCPSocket;
extern MQTT311_PrintPtr MQTT311_Print;

/* Function macros */
#define NULL_CHECK(pointer)             if (pointer == NULL){ MQTT311_Print("Pointer NULL Error"); }

/* The number of items the queue can hold.  */
#define mqttQUEUE_LENGTH			    128

/* Structure to keep user data */
struct UserData 
{
    /* Broker and user information */
    char* brokerAddress;
    char* socketID;
    char* local_port;

    /* Account information */
    char* username;
    char* password;
    char* deviceID;

    /* Broker information */
    uint16_t keepAlive;
    uint16_t port;

    uint8_t connect_flags;
};

/* MQTT Packets Enumerated */
enum MQTTPacketTypes 
{
    eCONNECT,
    ePUBLISH,
    eSUBSCRIBE,
    eUNSUBSCRIBE,
    ePINGREQ,
    eDISCONNECT
};

/* Structure used in queue signaling */
struct MQTTPacket
{
    /* Used to remember the location of itself for freeing purposes*/
    struct MQTTPacket *parent;

    union {
        /* Used to hold the pointer to the packet structure */
        struct CONNECT_MESSAGE *connect_message_structure;
        struct PUBLISH_MESSAGE *publish_message_structure;
        struct SUBSCRIBE_MESSAGE *subscribe_message_structure;
        struct UNSUBSCRIBE_MESSAGE *unsubscribe_message_structure;
        struct PINGREQ_MESSAGE *pingreq_message_structure;
        struct DISCONNECT_MESSAGE *disconnect_message_structure;
    }packet_data;

    /* Used to identify the package type */
    enum MQTTPacketTypes mqtt_packet_type;
};

/* Variable declarations */

/* UserData structure */
extern struct UserData userdata;

/* Bytes to send to function */
extern volatile char bytes_to_send[10000];
extern volatile char bytes_to_receive[10000];
extern uint16_t number_of_bytes_received; 

/* Variable used to keep track of indexes */
extern uint16_t current_index;

/* Queue Handler */
extern QueueHandle_t xMQTTQueue;

/* Function declarations */
void MQTT311_SetConnectTCPSocket(MQTT311_ConnectTCPSocketPtr connect_tcp_socket);
void MQTT311_SetCloseTCPSocket(MQTT311_CloseTCPSocketPtr close_tcp_socket);
void MQTT311_SetSendToTCPSocket(MQTT311_SendToTCPSocketPtr send_to_tcp_socket);
void MQTT311_SetReadFromTCPSocket(MQTT311_ReadFromTCPSocketPtr read_from_tcp_socket);
void MQTT311_SetPrint(MQTT311_PrintPtr print);
void MQTT311_SendMQTTPacket(struct MQTTPacket *mqtt_packet);
void MQTT311_CreateClient(const char* deviceID);
void MQTT311_SetUsernameAndPassword(const char* username, const char* password);
void MQTT311_EstablishConnectionToMQTTBroker(const char* brokerName, uint16_t port);
void MQTT311_AppendData(const char* data, uint16_t data_length, bool append_data_length);
void MQTT311_AppendTopicName(const char* topic_name);
void MQTT311_SendToMQTTBroker(uint16_t size);
void MQTT311_ReceiveFromMQTTBroker(void);
uint8_t MQTT311_EncodeRemainingLength(uint32_t length, uint8_t *encoded_bytes);
void MQTT311_CheckRemainingLength(void);
void MQTT311_MoveByteArrayToRight(uint8_t shift);
bool MQTT311_CheckResponseHeader(uint8_t packet_type, uint16_t remaining_length, uint8_t offset);
uint16_t MQTT311_GetPacketIdentifier(uint8_t offset);
bool MQTT311_GetPubPacketInfo(uint16_t packetIdentifier, uint8_t packet_type, uint16_t remaining_length, uint16_t offset);

#endif /* MQTT311_UTILITIES_H */
