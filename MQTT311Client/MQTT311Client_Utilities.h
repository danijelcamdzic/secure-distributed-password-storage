/**
 * @file MQTT311Client_Utilities.h
 * @brief Contains definitions, structures, and function declarations used by other MQTT 3.1.1 packets.
 * 
 * This file contains various definitions, structures, and function declarations used by other
 * MQTT 3.1.1 packets in the implementation. 
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_UTILITIES_H
#define MQTT311Client_UTILITIES_H

#include "MQTT311Client/MQTT311Client.h"

/* Macros for accessing the internal variables */
#define MQTT311_SEND_BUFFER     (MQTT311_SEND_BUFFER)
#define MQTT311_RECEIVE_BUFFER  (MQTT311_RECEIVE_BUFFER)
#define MQTT311_RECEIVED_BYTES  (MQTT311_RECEIVED_BYTES)

/* External functions for socket connection */
typedef void (*MQTT311Client_ConnectTCPSocketPtr)(const char*, uint16_t);
typedef void (*MQTT311Client_CloseTCPSocketPtr)(void);
typedef void (*MQTT311Client_SendToTCPSocketPtr)(const char*, uint16_t);
typedef void (*MQTT311Client_ReadFromTCPSocketPtr)(void);
extern MQTT311Client_ConnectTCPSocketPtr MQTT311Client_ConnectTCPSocket;
extern MQTT311Client_CloseTCPSocketPtr MQTT311Client_CloseTCPSocket;
extern MQTT311Client_SendToTCPSocketPtr MQTT311Client_SendToTCPSocket;
extern MQTT311Client_ReadFromTCPSocketPtr MQTT311Client_ReadFromTCPSocket;

/* External functions for debugging */
typedef void (*MQTT311Client_PrintPtr)(char*);
extern MQTT311Client_PrintPtr MQTT311Client_Print;

/* External functions for data processing */
typedef void (*MQTT311Client_ProcessBufferDataPtr)(void);
extern MQTT311Client_ProcessBufferDataPtr MQTT311Client_ProcessBufferData;

/* Function macros */
#define NULL_CHECK(pointer)             if (pointer == NULL){ MQTT311Client_Print("Pointer NULL Error"); }

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
extern volatile char MQTT311_SEND_BUFFER[10000];
extern volatile char MQTT311_RECEIVE_BUFFER[10000];
extern volatile uint16_t MQTT311_RECEIVED_BYTES; 

/* Variable used to keep track of indexes */
extern uint16_t current_index;

/* Queue Handler */
extern QueueHandle_t xMQTTQueue;

/* Function declarations */
void MQTT311Client_SetConnectTCPSocket(MQTT311Client_ConnectTCPSocketPtr connect_tcp_socket);
void MQTT311Client_SetCloseTCPSocket(MQTT311Client_CloseTCPSocketPtr close_tcp_socket);
void MQTT311Client_SetSendToTCPSocket(MQTT311Client_SendToTCPSocketPtr send_to_tcp_socket);
void MQTT311Client_SetReadFromTCPSocket(MQTT311Client_ReadFromTCPSocketPtr read_from_tcp_socket);
void MQTT311Client_SetPrint(MQTT311Client_PrintPtr print);
void MQTT311Client_SetProcessBufferData(MQTT311Client_ProcessBufferDataPtr process_buffer_data);
void MQTT311Client_SendMQTTPacket(struct MQTTPacket *mqtt_packet);
void MQTT311Client_CreateClient(const char* deviceID);
void MQTT311Client_SetUsernameAndPassword(const char* username, const char* password);
void MQTT311Client_EstablishConnectionToMQTTBroker(const char* brokerName, uint16_t port);
void MQTT311Client_AppendData(const char* data, uint16_t data_length, bool append_data_length);
void MQTT311Client_AppendTopicName(const char* topic_name);
void MQTT311Client_SendToMQTTBroker(uint16_t size);
void MQTT311Client_ReceiveFromMQTTBroker(void);
uint8_t MQTT311Client_EncodeRemainingLength(uint32_t length, uint8_t *encoded_bytes);
void MQTT311Client_CheckRemainingLength(void);
void MQTT311Client_MoveByteArrayToRight(uint8_t shift);
bool MQTT311Client_CheckResponseHeader(uint8_t packet_type, uint16_t remaining_length, uint8_t offset);
uint16_t MQTT311Client_GetPacketIdentifier(uint8_t offset);
bool MQTT311Client_GetPubPacketInfo(uint16_t packetIdentifier, uint8_t packet_type, uint16_t remaining_length, uint16_t offset);

#endif /* MQTT311Client_UTILITIES_H */
