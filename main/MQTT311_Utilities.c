/***********************************************************************
* FILENAME:        MQTT311_Utilities.c             
*
* DESCRIPTION:
*                  Contains helper functions that other MQTT 3.1.1 packets use
*                  in the implementation.
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

/* Included libraries */
#include "MQTT311.h"

/* Variable definitions */

/* Structure that keeps external functions that connect to the server */
// struct MQTTExtFunctions mqtt_ext_functions;
MQTT311_ConnectTCPSocketPtr MQTT311_ConnectTCPSocket = NULL;
MQTT311_SendToTCPSocketPtr MQTT311_SendToTCPSocket = NULL;
MQTT311_ReadFromTCPSocketPtr MQTT311_ReadFromTCPSocket = NULL;
MQTT311_PrintPtr MQTT311_Print = NULL;

/* UserData structure */
struct UserData userdata;

/* Bytes to send to function */
volatile char bytes_to_send[100];
volatile char bytes_to_receive[100];
uint16_t number_of_bytes_received = 0; 

/* Variable to keep track of indexes */
uint16_t current_index;

/* Queue used to hold MQTT packets */
QueueHandle_t xMQTTQueue = NULL;

/* Pointer to variable that keep the value of bytes left to read from socket */
// uint16_t* bytes_to_read = NULL;

/* Pointer to variable that monitors the connectivity */
// uint8_t* signal_monitoring_variable = NULL;

/* Private function declarations */
void MQTT311_SetConnectTCPSocket(MQTT311_ConnectTCPSocketPtr connect_tcp_socket)
{
    MQTT311_ConnectTCPSocket = connect_tcp_socket;
}
void MQTT311_SetSendToTCPSocket(MQTT311_SendToTCPSocketPtr send_to_tcp_socket)
{
    MQTT311_SendToTCPSocket = send_to_tcp_socket;
}
void MQTT311_SetReadFromTCPSocket(MQTT311_ReadFromTCPSocketPtr read_from_tcp_socket)
{
    MQTT311_ReadFromTCPSocket = read_from_tcp_socket;
}
void MQTT311_SetPrint(MQTT311_PrintPtr print) {
    MQTT311_Print = print;
}
// static void set_broker_address(const char* brokerAddress);
// static void set_port_number(uint16_t port);

/*
 * Function: set_close_socket_function
 * ----------------------------
 *   Sets the functions that closes a socket if necessary.
 *
 *   close_socket: function that closes a socket
 *
 *   returns: no return value
 */
// void set_close_socket_function(void (*close_socket)(const char*))
// {
//     mqtt_ext_functions.close_socket = close_socket;
// }

/*
 * Function: set_open_socket_function
 * ----------------------------
 *   Sets the functions that opens a socket if necessary.
 *
 *   open_socket: function that opens a socket
 *
 *   returns: no return value
 */
// void set_open_socket_function(void (*open_socket)(const char*, const char*))
// {
//     mqtt_ext_functions.open_socket = open_socket;
// }

/*
 * Function: set_connect_socket_function
 * ----------------------------
 *   Sets the functions that connects sockets.
 *
 *   connect_socket: function that connects a socket
 *
 *   returns: no return value
 */
// void set_connect_socket_function(void (*connect_socket)(const char*, const char*, const char*))
// {
//     mqtt_ext_functions.connect_socket = connect_socket;
// }

/*
 * Function: set_send_data_to_socket_function
 * ----------------------------
 *   Sets the functions that sends data over socket connection.
 *
 *   send_data_to_socket: function that sends data to socket
 *
 *   returns: no return value
 */
// void set_send_data_to_socket_function(void (*send_data_to_socket)(const char*, const char*, const char*, int))
// {
//     mqtt_ext_functions.send_data_to_socket = send_data_to_socket;
// }

/*
 * Function: set_send_char_to_socket_function
 * ----------------------------
 *   Sets the functions that sends char over socket connection.
 *
 *   set_send_char_to_socket_function: function that sends char to socket
 *
 *   returns: no return value
 */
// void set_send_char_to_socket_function(void (*send_char_to_socket)(char))
// {
//     mqtt_ext_functions.send_char_to_socket = send_char_to_socket;
// }

/*
 * Function: set_read_data_from_socket_function
 * ----------------------------
 *   Sets the functions that reads data from socket.
 *
 *   set_read_data_from_socket_function: function that reads data from socket
 *
 *   returns: no return value
 */
// void set_read_data_from_socket_function(void (*read_data_from_socket)(const char*, const char*))
// {
//     mqtt_ext_functions.read_data_from_socket = read_data_from_socket;
// }

/*
 * Function: set_get_response_byte_function
 * ----------------------------
 *   Sets the functions that retrieves a byte from the server response.
 *
 *   get_response_byte: function that fetches byte from the response data
 *
 *   returns: no return value
 */
// void set_get_response_byte_function(uint16_t (*get_response_byte)(uint8_t))
// {
//     mqtt_ext_functions.get_response_byte = get_response_byte;
// }

/*
 * Function: set_monitor_connection_function
 * ----------------------------
 *   Monitors connection that the module has.
 *
 *
 *   returns: no return value
 */
// void set_monitor_connection_function(void (*monitor_connection)(void))
// {
//     mqtt_ext_functions.monitor_connection = monitor_connection;
// }

/*
 * Function: set_mqtt_external_functions
 * ----------------------------
 *   Sets all the functions that the mqtt protocol needs to communicate with the server.
 *
 *   close_socket: function that closes a socket
 *   connect_socket: function that connects a socket
 *   send_data_to_socket: function that sends data to socket
 *   set_send_char_to_socket_function: function that sends char to socket
 *   set_read_data_from_socket_function: function that reads data from socket
 *   get_response_byte: function that fetches byte from the response data
 *
 *   returns: no return value
 */
// void set_mqtt_external_functions(
//     void (*close_socket)(const char*),
//     void (*open_socket)(const char*, const char*),
//     void (*connect_socket)(const char*, const char*, const char*),
//     void (*send_data_to_socket)(const char*, const char*, const char*, int),
//     void (*send_char_to_socket)(char),
//     void (*read_data_from_socket)(const char*, const char*),
//     uint16_t (*get_response_byte)(uint8_t),
//     void (*monitor_connection)(void)
// )
// {
//     /* Call functions specific to input parameter */
//     set_close_socket_function(close_socket);
//     set_open_socket_function(open_socket);
//     set_connect_socket_function(connect_socket);
//     set_send_data_to_socket_function(send_data_to_socket);
//     set_send_char_to_socket_function(send_char_to_socket);
//     set_read_data_from_socket_function(read_data_from_socket);
//     set_get_response_byte_function(get_response_byte);
//     mqtt_ext_functions.monitor_connection = monitor_connection;

// }

/*
 * Function: reconnection_sequence
 * ----------------------------
 *   Tries to reconnect to broker by closing and opening the socket and reconnecting.
 *
 *   returns: no return value
 */
// void reconnection_sequence()
// {
//     /* Close socket */
//     mqtt_ext_functions.close_socket(userdata.socketID);

//     /* Open socket */
//     mqtt_ext_functions.open_socket(OPEN_SOCKET_CODE, userdata.local_port);

//     /* Connect to broker */
//     connect_to_broker(userdata.socketID, userdata.brokerAddress, userdata.port, userdata.local_port);

//     /* Send connect message */
//     connect(userdata.connect_flags, userdata.keepAlive, "", "");
// }

/*
 * Function: MQTT311_SendMQTTPacket
 * ----------------------------
 *   Sends the received mqtt packet into the appropriate function from the structure.
 *
 *   mqtt_packet: mqtt packet that is to be sent.
 *
 *   returns: no return value
 */
void MQTT311_SendMQTTPacket(struct MQTTPacket *mqtt_packet)
{
    /* Checks type of packet and sends using appropriate structure */
    switch(mqtt_packet->mqtt_packet_type) 
    {
        case eCONNECT:
            mqtt_packet->packet_data.connect_message_structure->MQTT311_ConnectWithStruct(mqtt_packet->packet_data.connect_message_structure);
            vPortFree(mqtt_packet->parent);
            break;

        case ePUBLISH:
            mqtt_packet->packet_data.publish_message_structure->MQTT311_PublishWithStruct(mqtt_packet->packet_data.publish_message_structure);
            vPortFree(mqtt_packet->parent);
            break;

        case eSUBSCRIBE:
            mqtt_packet->packet_data.subscribe_message_structure->MQTT311_SubscribeWithStruct(mqtt_packet->packet_data.subscribe_message_structure);
            vPortFree(mqtt_packet->parent);
            break;

        // case eUNSUBSCRIBE:
        //     mqtt_packet->packet_data.unsubscribe_message_structure->unMQTT311_SubscribeWithStruct(mqtt_packet->packet_data.unsubscribe_message_structure);
        //     vPortFree(mqtt_packet->parent);
        //     break;

        // case ePINGREQ:
        //     mqtt_packet->packet_data.pingreq_message_structure->pingreq_with_struct(mqtt_packet->packet_data.pingreq_message_structure);
        //     vPortFree(mqtt_packet->parent);
        //     break;

        // case eDISCONNECT:
        //     mqtt_packet->packet_data.disconnect_message_structure->disMQTT311_ConnectWithStruct(mqtt_packet->packet_data.disconnect_message_structure);
        //     vPortFree(mqtt_packet->parent);
        //     break;
        default:
            break;

    }
}

/*
 * Function: set_bytes_to_read_variable
 * ----------------------------
 *   Keeps the memory address of the variable that keeps remaining bytes to read from socket.
 *
 *   bytesToRead: pointer to address of the variable that keeps information
 *                on the ammount of bytes left to read.
 *
 *   returns: no return value
 */
// void set_bytes_to_read_variable(uint16_t* bytesToRead)
// {
//     bytes_to_read = bytesToRead;
// }

/*
 * Function: set_connection_monitoring_variable
 * ----------------------------
 *   Keeps the memory address of the variable that monitors connectivity.
 *
 *   monitoring_variable: Sets the variable which is used to monitor the connectivity.
 *
 *   returns: no return value
 */
// void set_connection_monitoring_variable(uint16_t* monitoring_variable)
// {
//     signal_monitoring_variable = monitoring_variable;
// }

/*
 * Function: set_broker_address
 * ----------------------------
 *   Sets the Cumulocity broker address.
 *
 *   brokerAddress: Cumulocity broker address
 *
 *   returns: no return value
 */
// static void set_broker_address(const char* brokerAddress)
// {
//     /* If already allocated free the memory */
//     if (userdata.brokerAddress != NULL) {
//         return;
//     }

//     userdata.brokerAddress = (char*) pvPortMalloc(strlen(brokerAddress)+1);
//     NULL_CHECK(userdata.brokerAddress)
//     memcpy(userdata.brokerAddress, brokerAddress, strlen(brokerAddress)+1);
// }

/*
 * Function: set_port_number
 * ----------------------------
 *   Sets the port number to connect to Cumulocity.
 *
 *   port: port number
 *
 *   returns: no return value
 */
// static void set_port_number(uint16_t port) 
// {
//     userdata.port = port;
// }

/*
 * Function: MQTT311_CreateClient
 * ----------------------------
 *   Creates client by giving it device id.
 *
 *   deviceID: device id of the client
 *
 *   returns: no return value
 */
void MQTT311_CreateClient(const char* deviceID) 
{
    /* If already allocated free the memory */
    if (userdata.deviceID != NULL) {
        return;
    }

    userdata.deviceID = (char*) pvPortMalloc(strlen(deviceID)+1);
    NULL_CHECK(userdata.deviceID)
    memcpy(userdata.deviceID, deviceID, strlen(deviceID)+1);
}

/*
 * Function: MQTT311_SetUsernameAndPassword
 * ----------------------------
 *   Sets the username and password into the userdata structure.
 *
 *   username: client's username
 *   password: client's password
 *
 *   returns: no return value
 */
void MQTT311_SetUsernameAndPassword(const char* username, const char* password) 
{
    /* If already allocated free the memory */
    if (userdata.username != NULL) {
        return;
    }

    userdata.username = (char*) pvPortMalloc(strlen(username)+1);
    NULL_CHECK(userdata.username)
    memcpy(userdata.username, username, strlen(username)+1);

    /* If already allocated free the memory */
    if (userdata.password != NULL) {
        return;
    }

    userdata.password = (char*) pvPortMalloc(strlen(password)+1);
    NULL_CHECK(userdata.password)
    memcpy(userdata.password, password, strlen(password)+1);
}

/*
 * Function: set_local_port_number
 * ----------------------------
 *   Sets the home port number
 *
 *   local_port: port number of the local pc
 *
 *   returns: no return value
 */
// void set_local_port_number(const char* local_port) 
// {
//     /* If already allocated free the memory */
//     if (userdata.local_port != NULL) {
//         return;
//     }

//     userdata.local_port = (char*) pvPortMalloc(strlen(local_port)+1);
//     NULL_CHECK(userdata.local_port)
//     memcpy(userdata.local_port, local_port, strlen(local_port)+1);
// }

/*
 * Function: set_socket_identifier
 * ----------------------------
 *   Sets the socket identifier
 *
 *   socketID: socket identifier
 *
 *   returns: no return value
 */
// void set_socket_identifier(const char* socketIdentifier) 
// {
//     /* If already allocated free the memory */
//     if (userdata.socketID != NULL) {
//         return;
//     }

//     userdata.socketID = (char*) pvPortMalloc(strlen(socketIdentifier)+1);
//     NULL_CHECK(userdata.socketID)
//     memcpy(userdata.socketID, socketIdentifier, strlen(socketIdentifier)+1);
// }

/*
 * Function: MQTT_EstablishConnectionToMQTTBroker
 * ----------------------------
 *   Establishes a TCP connection to the broker over a tcp port given.
 *
 *   socketIdentifier: socket identifier
 *   brokerAddress: broker address of Cumulocity
 *   port: port number over which the TCP connection is to be established
 *
 *   returns: no return value
 */
void MQTT_EstablishConnectionToMQTTBroker(const char* brokerName, uint16_t port) 
{
    MQTT311_ConnectTCPSocket(brokerName, port);
}

/*
 * Function: MQTT311_AppendData
 * ----------------------------
 *   Appends data and data length to byte array.
 *
 *   data: data to be appended
 *   data_length: data length to be appended
 *   append_data_length: information on whether to append data length or not
 *
 *   returns: no return value
 */
void MQTT311_AppendData(const char* data, uint16_t data_length, bool append_data_length)
{

    /* Appends data length to byte array */
    if (append_data_length)
    {
        bytes_to_send[current_index++] = data_length >> 8;
        bytes_to_send[current_index++] = data_length & 0xFF;
    }
    /* Appends data to byte array */
    memcpy(((void*)bytes_to_send) + current_index, data, data_length);
    current_index += data_length;
}

/*
 * Function: MQTT311_AppendTopicName
 * ----------------------------
 *   Appends topic_name to the MQTT message
 *
 *   topic_name: name of the topic that is published
 *
 *   returns: no return value
 */
void MQTT311_AppendTopicName(const char* topic_name) 
{
    uint16_t topicnameLength = strlen(topic_name);

    MQTT311_AppendData(topic_name, topicnameLength, true);

}

/*
 * Function: MQTT311_SendToMQTTBroker
 * ----------------------------
 *   Connects to Cumulocity tenant by providing user's username and password
 *
 *   size: size of the message to send 
 *
 *   returns: no return value
 */
void MQTT311_SendToMQTTBroker(uint16_t size) 
{
    MQTT311_SendToTCPSocket((const char *)bytes_to_send, size);
}
void MQTT311_ReceiveFromMQTTBroker(void) 
{
    MQTT311_ReadFromTCPSocket((char*)bytes_to_receive, &number_of_bytes_received);
}


/*
 * Function: MQTT311_EncodeRemainingLength
 * ----------------------------
 *   Encodes remaining length if larger than 127.
 *
 *   length: remaining length of the message.
 *
 *   returns: uint8_t remaining byte
 */
uint8_t MQTT311_EncodeRemainingLength(uint16_t length) 
{
    /* Encoded byte */
    uint16_t encodedByte = 0x0000;

    /* Finding the encoded byte */
    while(length > 0)
    {
        encodedByte = length % 128;
        length = length / 128;
        if (length > 0) 
        {
        encodedByte = encodedByte | 128;
        }
    }
    return encodedByte;
}

/*
 * Function: MQTT311_CheckRemainingLength
 * ----------------------------
 *   Checks if remaining length value needs to be encoded
 *
 *   returns: uint8_t remaining length value
 */
uint8_t MQTT311_CheckRemainingLength(void)
{
    uint8_t remaining_length;

    /* Check if remaining length value is greater than 127 */
    if ((current_index - 2) > REMAINING_LENGTH_MAX)
    {
        uint8_t encodedByte = MQTT311_EncodeRemainingLength(current_index);
        uint8_t remainder = current_index - encodedByte*128;
        remaining_length = remainder + 128;
        MQTT311_MoveByteArrayToLeft();
        bytes_to_send[2] = encodedByte;
    }
    else 
    {
        remaining_length = current_index - 2;
    }

    return remaining_length;
}

/*
 * Function: move_byte_array_to_left
 * ----------------------------
 *   Moves entire bytearray to the left by one block.
 *
 *   returns: no return value
 */
void MQTT311_MoveByteArrayToLeft(void)
{
    /* Moving the array */
    for (int i = current_index; i > 1; i--)
    {
        bytes_to_send[i] = bytes_to_send[i+1];
    }
}

/*
 * Function: MQTT311_CheckResponseHeader
 * ----------------------------
 *   Checks whether the appropriate response was received.
 *
 *   packet_type: type of MQTT packet
 *   remainingLength: remaining length of the MQTT packet
 *   offset: starting index from which the response is read
 * 
 *   returns: information on whether the proper response was received
 */
bool MQTT311_CheckResponseHeader(uint8_t packet_type, uint16_t remainingLength, uint8_t offset)
{
    uint8_t server_response_header;

    /* Getting proper bytes from the header */
    server_response_header = bytes_to_receive[offset];

    /* Checking the response header */
    if (server_response_header != packet_type)
    {
        return false;
    }

    /* Getting remaining length */
    uint8_t remaining_length = bytes_to_receive[offset + 1];

    /* Checking the remaining length */
    if (remaining_length != remainingLength)
    {
        return false;
    }

    return true;
}

/*
 * Function: MQTT311_GetPacketIdentifier
 * ----------------------------
 *  Gets the packet_identifier
 *
 *  offset: starting index from which the response is read
 * 
 *  returns: packet_identifier
 */
uint16_t MQTT311_GetPacketIdentifier(uint8_t offset)
{

    /* Getting packet identifier */
    uint16_t packet_identifier = (bytes_to_receive[offset] << 8) | (bytes_to_receive[offset + 1]);

    return packet_identifier;
}

/*
 * Function: get_pub_receive_packet_info
 * ----------------------------
 *  Checks whether the pub receive packet parts indicate a succesfull response
 *
 *  packetIdentifier: packet ID of the publish packet
 *  packet_type: type of receive packet
 *  remaining_length: remaining length of the packet
 *  offset: starting index from which the response is read
 * 
 *  returns: bool
 */
// bool get_pub_receive_packet_info(uint16_t packetIdentifier, uint8_t packet_type, uint16_t remaining_length, uint16_t offset) 
// {
//     /* Useful flag for keeping track of sucess of response message */
//     bool success_message = true;

//     for (uint8_t i = 0; i < offset+1; i++)
//     {
//         success_message = true;

//         /* Check if correct response package was received */
//         bool correct_response = check_response_headers(packet_type, remaining_length, i, false);

//         /* If incorrect reponse package, return false */
//         if (!correct_response)
//         {
//             success_message = false;
//             continue;
//         }

//         uint16_t packet_identifier = get_packet_identifier(i);

//         /* Checking if this is the correct packet identifier */
//         if (packet_identifier != packetIdentifier) 
//         {
//             success_message = false;
//         }
//         else
//         {
//             break;
//         }
//     }

//     return success_message;
// }