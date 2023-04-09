/***********************************************************************
* FILENAME:        MQTT311_Utilities.h             
*
* DESCRIPTION:
*                  Contains definitions, structure and function declarations
*                  which other MQTT 3.1.1 packets use in the implementation.
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

#ifndef MQTT311_UTILITIES_H
#define MQTT311_UTILITIES_H

/* Necessary inclusion */
#include "MQTT311/MQTT311.h"

/* Function macros */
#define NULL_CHECK(pointer)             if (pointer == NULL){/* printf("\r\nFailed to allocate memory for the connect_message_data\r\n")*/;}

/* The number of items the queue can hold.  */
#define mqttQUEUE_LENGTH			    128

/* External Function */
typedef void (*MQTT311_ConnectTCPSocketPtr)(const char*, uint16_t);
typedef void (*MQTT311_SendToTCPSocketPtr)(const char*, uint16_t);
typedef void (*MQTT311_ReadFromTCPSocketPtr)(char*, uint16_t*);
typedef void (*MQTT311_PrintPtr)(char*);

extern MQTT311_ConnectTCPSocketPtr MQTT311_ConnectTCPSocket;
extern MQTT311_SendToTCPSocketPtr MQTT311_SendToTCPSocket;
extern MQTT311_ReadFromTCPSocketPtr MQTT311_ReadFromTCPSocket;
extern MQTT311_PrintPtr MQTT311_Print;

/* Define statement that is used in AT command socket opening */
// #define OPEN_SOCKET_CODE                "6"

/* Structure that contains functions for socket communication */
// struct MQTTExtFunctions 
// {
//     /* Functions */
//     void (*close_socket)(char*);                           /* Function that closes a socket - not necessary in some implementations */
//     void (*open_socket)(char*, char*);                     /* Function that open a socket - not necessary in some implementations */
//     void (*connect_socket)(char*, char*, char*);           /* Function that connects socket */
//     void (*send_data_to_socket)(char*, char*, char*, int); /* Write data to socket */
//     void (*send_char_to_socket)(char);                     /* Function that sends char */
//     void (*read_data_from_socket)(char*, char*);           /* Function that reads the response from the server */
//     uint16_t (*get_response_byte)(uint8_t);                /* Function that gets a byte from the response */
//     void (*monitor_connection)(void);                      /* Monitors the connection the module has with the internet */
// };

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

/* Function structure */
// extern struct MQTTExtFunctions mqtt_ext_functions;

/* UserData structure */
extern struct UserData userdata;

/* Bytes to send to function */
extern volatile char bytes_to_send[100];
extern volatile char bytes_to_receive[100];
extern uint16_t number_of_bytes_received; 

/* Variable used to keep track of indexes */
extern uint16_t current_index;

/* Queue Handler */
extern QueueHandle_t xMQTTQueue;

/* Pointer to variable that keep the value of bytes left to read from socket */
// extern uint16_t* bytes_to_read;
/* Pointer to variable that monitors the connectivity */
// extern uint8_t* signal_monitoring_variable;

/* Function declarations */
void MQTT311_SetConnectTCPSocket(MQTT311_ConnectTCPSocketPtr connect_tcp_socket);
void MQTT311_SetSendToTCPSocket(MQTT311_SendToTCPSocketPtr send_to_tcp_socket);
void MQTT311_SetReadFromTCPSocket(MQTT311_ReadFromTCPSocketPtr read_from_tcp_socket);
void MQTT311_SetPrint(MQTT311_PrintPtr print);
// void set_mqtt_external_functions(
//     void (*close_socket)(const char*),
//     void (*open_socket)(const char*, const char*),
//     void (*connect_socket)(const char*, const char*, const char*),
//     void (*send_data_to_socket)(const char*, const char*, const char*, int),
//     void (*send_char_to_socket)(char),
//     void (*read_data_from_socket)(const char*, const char*),
//     uint16_t (*get_response_byte)(uint8_t),
//     void (*monitor_connection)(void)
// );
// void set_close_socket_function(void (*close_socket)(const char*));
// void set_open_socket_function(void (*open_socket)(const char*, const char*));
// void set_connect_socket_function(void (*connect_socket)(const char*, const char*, const char*));
// void set_send_data_to_socket_function(void (*send_data_to_socket)(const char*, const char*, const char*, int));
// void set_send_char_to_socket_function(void (*send_char_to_socket)(char));
// void set_read_data_from_socket_function(void (*read_data_from_socket)(const char*, const char*));
// void set_get_response_byte_function(uint16_t (*get_response_byte)(uint8_t));
// void set_monitor_connection_function(void (*monitor_connection)(void));
// void reconnection_sequence(void);
void MQTT311_SendMQTTPacket(struct MQTTPacket *mqtt_packet);
// void set_bytes_to_read_variable(uint16_t* bytesToRead);
// void set_connection_monitoring_variable(uint16_t* monitoring_variable);
void MQTT311_CreateClient(const char* deviceID);
void MQTT311_SetUsernameAndPassword(const char* username, const char* password);
void MQTT_EstablishConnectionToMQTTBroker(const char* brokerName, uint16_t port);
void MQTT311_AppendData(const char* data, uint16_t data_length, bool append_data_length);
void MQTT311_AppendTopicName(const char* topic_name);
void MQTT311_SendToMQTTBroker(uint16_t size);
void MQTT311_ReceiveFromMQTTBroker(void);
uint8_t MQTT311_EncodeRemainingLength(uint16_t length);
uint8_t MQTT311_CheckRemainingLength(void);
void MQTT311_MoveByteArrayToLeft(void);
bool MQTT311_CheckResponseHeader(uint8_t packet_type, uint16_t remaining_length, uint8_t offset);
uint16_t MQTT311_GetPacketIdentifier(uint8_t offset);
bool MQTT311_GetPubPacketInfo(uint16_t packetIdentifier, uint8_t packet_type, uint16_t remaining_length, uint16_t offset);

#endif
