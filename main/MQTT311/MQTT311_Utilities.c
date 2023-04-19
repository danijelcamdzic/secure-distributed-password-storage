/**
 * @file MQTT311_Utilities.c
 * @brief Contains helper functions that other MQTT 3.1.1 packets use in the implementation.
 * 
 * This file contains various helper functions used by other MQTT 3.1.1 packets in the implementation. 
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311/MQTT311.h"

/* Variable definitions */

/* Structure that keeps external functions that connect to the server */
MQTT311_ConnectTCPSocketPtr MQTT311_ConnectTCPSocket = NULL;
MQTT311_CloseTCPSocketPtr MQTT311_CloseTCPSocket = NULL;
MQTT311_SendToTCPSocketPtr MQTT311_SendToTCPSocket = NULL;
MQTT311_ReadFromTCPSocketPtr MQTT311_ReadFromTCPSocket = NULL;
MQTT311_PrintPtr MQTT311_Print = NULL;
MQTT311_ProcessBufferDataPtr MQTT311_ProcessBufferData = NULL;

/* UserData structure */
struct UserData userdata;

/* Bytes to send to function */
volatile char bytes_to_send[10000];
volatile char bytes_to_receive[10000];
uint16_t number_of_bytes_received = 0; 

/* Variable to keep track of indexes */
uint16_t current_index;

/* Queue used to hold MQTT packets */
QueueHandle_t xMQTTQueue = NULL;

/* Private function declarations */
static void MQTT311_SetBrokerAddress(const char* brokerAddress);
static void MQTT311_SetPortNumber(uint16_t port);

/* Setting the external functions */
/**
 * @brief Sets the MQTT 3.1.1 Connect TCP Socket function.
 * @param connect_tcp_socket Function pointer to the user-defined MQTT 3.1.1 Connect TCP Socket function.
 */
void MQTT311_SetConnectTCPSocket(MQTT311_ConnectTCPSocketPtr connect_tcp_socket)
{
    MQTT311_ConnectTCPSocket = connect_tcp_socket;
}

/**
 * @brief Sets the MQTT 3.1.1 Close TCP Socket function.
 * @param close_tcp_socket Function pointer to the user-defined MQTT 3.1.1 Close TCP Socket function.
 */
void MQTT311_SetCloseTCPSocket(MQTT311_CloseTCPSocketPtr close_tcp_socket)
{
    MQTT311_CloseTCPSocket = close_tcp_socket;
}

/**
 * @brief Sets the MQTT 3.1.1 Send to TCP Socket function.
 * @param send_to_tcp_socket Function pointer to the user-defined MQTT 3.1.1 Send to TCP Socket function.
 */
void MQTT311_SetSendToTCPSocket(MQTT311_SendToTCPSocketPtr send_to_tcp_socket)
{
    MQTT311_SendToTCPSocket = send_to_tcp_socket;
}

/**
 * @brief Sets the MQTT 3.1.1 Read from TCP Socket function.
 * @param read_from_tcp_socket Function pointer to the user-defined MQTT 3.1.1 Read from TCP Socket function.
 */
void MQTT311_SetReadFromTCPSocket(MQTT311_ReadFromTCPSocketPtr read_from_tcp_socket)
{
    MQTT311_ReadFromTCPSocket = read_from_tcp_socket;
}

/**
 * @brief Sets the MQTT 3.1.1 Print function.
 * @param print Function pointer to the user-defined MQTT 3.1.1 Print function.
 */
void MQTT311_SetPrint(MQTT311_PrintPtr print) 
{
    MQTT311_Print = print;
}

/**
 * @brief Sets the MQTT 3.1.1 process buffer data function.
 * @param print Function pointer to the user-defined MQTT 3.1.1 process buffer data function.
 */
void MQTT311_SetProcessBufferData(MQTT311_ProcessBufferDataPtr process_buffer_data)
{
    MQTT311_ProcessBufferData = process_buffer_data;
}

/**
 * @brief Sends the received MQTT packet into the appropriate function from the structure.
 *
 * This function is used to send the received MQTT packet into the appropriate function from the 
 * structure. It takes a pointer to an MQTT packet structure as input and does not return any value.
 * 
 * @param mqtt_packet The MQTT packet that is to be sent.
 *
 * @return None.
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

        case eUNSUBSCRIBE:
            mqtt_packet->packet_data.unsubscribe_message_structure->MQTT311_UnsubscribeWithStruct(mqtt_packet->packet_data.unsubscribe_message_structure);
            vPortFree(mqtt_packet->parent);
            break;

        case ePINGREQ:
            mqtt_packet->packet_data.pingreq_message_structure->MQTT311_PingreqWithStruct(mqtt_packet->packet_data.pingreq_message_structure);
            vPortFree(mqtt_packet->parent);
            break;

        case eDISCONNECT:
            mqtt_packet->packet_data.disconnect_message_structure->MQTT311_DisconnectWithStruct(mqtt_packet->packet_data.disconnect_message_structure);
            vPortFree(mqtt_packet->parent);
            break;
        default:
            break;
    }
}

/**
 * @brief Sets the MQTT Broker broker address.
 *
 * This function is used to set the MQTT Broker broker address. It takes the address as a 
 * string input and does not return any value.
 * 
 * @param brokerAddress The MQTT Broker broker address.
 *
 * @return None.
 */ 
static void MQTT311_SetBrokerAddress(const char* brokerAddress)
{
    /* If already allocated free the memory */
    if (userdata.brokerAddress != NULL) {
        return;
    }

    userdata.brokerAddress = (char*) pvPortMalloc(strlen(brokerAddress)+1);
    NULL_CHECK(userdata.brokerAddress)
    memcpy(userdata.brokerAddress, brokerAddress, strlen(brokerAddress)+1);
}

/**
 * @brief Sets the port number to connect to MQTT Broker.
 *
 * This function is used to set the port number to connect to MQTT Broker. It takes the port number as 
 * input and does not return any value.
 * 
 * @param port The port number.
 *
 * @return None.
 */ 
static void MQTT311_SetPortNumber(uint16_t port) 
{
    userdata.port = port;
}

/**
 * @brief Creates a client by giving it a device ID.
 *
 * This function is used to create a client by giving it a device ID. It takes the device ID as a string 
 * input and does not return any value.
 * 
 * @param deviceID The device ID of the client.
 *
 * @return None.
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

/**
 * @brief Sets the username and password into the userdata structure.
 *
 * This function is used to set the username and password into the userdata structure. It takes the 
 * client's username and password as string inputs and does not return any value.
 * 
 * @param username The client's username.
 * @param password The client's password.
 *
 * @return None.
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

/**
 * @brief Establishes a TCP connection to the MQTT broker over a TCP port.
 *
 * This function is used to establish a TCP connection to the MQTT broker over a TCP port.
 * It takes the broker address as a string input, the port number as a uint16_t input, and
 * does not return any value.
 * 
 * @param brokerName The broker address of the MQTT broker.
 * @param port The port number over which the TCP connection is to be established.
 *
 * @return None.
 */ 
void MQTT311_EstablishConnectionToMQTTBroker(const char* brokerName, uint16_t port) 
{
    MQTT311_SetBrokerAddress(brokerName);
    MQTT311_SetPortNumber(port);
    MQTT311_ConnectTCPSocket(brokerName, port);
}

/**
 * @brief Appends the data and data length to the byte array.
 *
 * This function is used to append the data and data length to the byte array. It takes the data to be 
 * appended and its length as inputs, as well as a bool indicating whether to append the data length or 
 * not. The function does not return any value.
 * 
 * @param data The data to be appended.
 * @param data_length The length of the data to be appended.
 * @param append_data_length A bool indicating whether to append the data length or not.
 *
 * @return None.
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

/**
 * @brief Appends the topic name to the MQTT message.
 *
 * This function is used to append the topic name to the MQTT message. It takes the name of the topic 
 * that is being published as a string input and does not return any value.
 * 
 * @param topic_name The name of the topic that is being published.
 *
 * @return None.
 */ 
void MQTT311_AppendTopicName(const char* topic_name) 
{
    uint16_t topicnameLength = strlen(topic_name);

    MQTT311_AppendData(topic_name, topicnameLength, true);

}

/**
 * @brief Sends a message to the MQTT Broker.
 *
 * This function is used to send a message to the MQTT Broker. It takes the size of the message as a 
 * uint16_t input and does not return any value.
 * 
 * @param size The size of the message to send.
 *
 * @return None.
 */ 
void MQTT311_SendToMQTTBroker(uint16_t size) 
{
    MQTT311_SendToTCPSocket((const char *)bytes_to_send, size);
}

/**
 * @brief Receives a message from the MQTT Broker.
 *
 * This function is used to receive a message from the MQTT Broker. The message bytes are stored in
 * bytes_to_receive and the number of bytes received is stored in number_of_bytes_received.
 * 
 * @param None
 *
 * @return None.
 */ 
void MQTT311_ReceiveFromMQTTBroker(void) 
{
    MQTT311_ReadFromTCPSocket((char*)bytes_to_receive, &number_of_bytes_received);
}

/**
 * @brief Encodes the remaining length if larger than 127.
 *
 * This function is used to encode the remaining length according to the MQTT 3.1.1 specification. 
 * It takes the remaining length of the message as a uint32_t input and encodes it into an array of bytes.
 * 
 * @param length The remaining length of the message.
 * @param encoded_bytes The encoded bytes as an array of uint8_t.
 * @return The number of bytes used for encoding.
 */ 
uint8_t MQTT311_EncodeRemainingLength(uint32_t length, uint8_t *encoded_bytes) 
{
    uint8_t byte_index = 0;

    do
    {
        encoded_bytes[byte_index] = length % 128;
        length = length / 128;

        if (length > 0)
        {
            encoded_bytes[byte_index] |= 128;
        }
        byte_index++;
    } while (length > 0);

    return byte_index;
}

/**
 * @brief Checks if the remaining length value needs to be encoded and updates the bytes_to_send array.
 *
 * This function is used to check if the remaining length value needs to be encoded according to the MQTT 3.1.1 specification.
 * It encodes the remaining length value and updates the bytes_to_send array accordingly.
 * 
 * @return None.
 */ 
void MQTT311_CheckRemainingLength(void)
{
    uint32_t remaining_length = current_index - 2;
    uint8_t encoded_bytes[4];

    uint8_t num_encoded_bytes = MQTT311_EncodeRemainingLength(remaining_length, encoded_bytes);

    if (num_encoded_bytes > 1) {
        MQTT311_MoveByteArrayToRight(num_encoded_bytes);

        for (uint8_t i = 0; i < num_encoded_bytes; i++)
        {
            bytes_to_send[i + 1] = encoded_bytes[i];
        }

        current_index += num_encoded_bytes - 1;
    }
    else {
        remaining_length = current_index - 2;
        bytes_to_send[1] = remaining_length;
    }
}

/**
 * @brief Moves the entire byte array to the right by the specified number of positions.
 *
 * This function is used to move the entire byte array to the right by the specified number of positions. 
 * It does not return any value.
 * 
 * @param shift The number of positions to shift the array to the right.
 * @return None.
 */ 
void MQTT311_MoveByteArrayToRight(uint8_t shift)
{
    /* Moving the array */
    for (int i = current_index; i > 1; i--)
    {
        bytes_to_send[i + shift - 1] = bytes_to_send[i];
    }
}

/**
 * @brief Checks whether the appropriate response was received.
 *
 * This function is used to check whether the appropriate response was received. It takes the type 
 * of MQTT packet, the remaining length of the MQTT packet, and the starting index from which the 
 * response is read as inputs. The function returns a bool indicating whether the proper response 
 * was received or not.
 * 
 * @param packet_type The type of MQTT packet.
 * @param remainingLength The remaining length of the MQTT packet.
 * @param offset The starting index from which the response is read.
 *
 * @return A bool indicating whether the proper response was received or not.
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

/**
 * @brief Gets the packet identifier.
 *
 * This function is used to get the packet identifier. It takes the starting index from which the 
 * response is read as input and returns the packet identifier.
 * 
 * @param offset The starting index from which the response is read.
 *
 * @return The packet identifier.
 */ 
uint16_t MQTT311_GetPacketIdentifier(uint8_t offset)
{

    /* Getting packet identifier */
    uint16_t packet_identifier = (bytes_to_receive[offset] << 8) | (bytes_to_receive[offset + 1]);

    return packet_identifier;
}

/**
 * @brief Checks whether the pub receive packet parts indicate a successful response.
 *
 * This function is used to check whether the received pub packet parts indicate a successful response.
 * It takes the packet ID of the publish packet, the type of receive packet, the remaining length of the packet, 
 * and the starting index from which the response is read as inputs. The function returns a bool indicating 
 * whether the response was successful or not.
 * 
 * @param packetIdentifier The packet ID of the publish packet.
 * @param packet_type The type of receive packet.
 * @param remaining_length The remaining length of the packet.
 * @param offset The starting index from which the response is read.
 *
 * @return bool indicating whether the response was successful or not.
 */ 
bool MQTT311_GetPubPacketInfo(uint16_t packetIdentifier, uint8_t packet_type, uint16_t remaining_length, uint16_t offset) 
{
    /* Useful flag for keeping track of sucess of response message */
    bool success_message = true;

    success_message = true;

    /* Check if correct response package was received */
    bool correct_response = MQTT311_CheckResponseHeader(packet_type, remaining_length, 0);

    /* If incorrect reponse package, return false */
    if (!correct_response)
    {
        success_message = false;
        return success_message;
    }

    uint16_t packet_identifier = MQTT311_GetPacketIdentifier(2);

    /* Checking if this is the correct packet identifier */
    if (packet_identifier != packetIdentifier) 
    {
        success_message = false;
        return success_message;
    }

    return success_message;
}