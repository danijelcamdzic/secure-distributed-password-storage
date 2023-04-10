/**
 * @file MQTT311_Connect.c
 * @brief Contains variable and function definitions for the MQTT 3.1.1 CONNECT package.
 *
 * This file contains the variable and function definitions required for implementing the MQTT 3.1.1 CONNECT package.
 * The CONNECT package is used to establish a connection between an MQTT client and an MQTT broker.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311/MQTT311.h"

/* Private function declarations */
static void MQTT311_SetKeepAlive(uint16_t keepalive);
static void MQTT311_AppendClientID(const char* client_id);
static void MQTT311_AppendUsernameAndPassword(void);
static void MQTT311_AppendWillTopic(const char* will_topic);
static void MQTT311_AppendWillMessage(const char* will_message);
static void MQTT311_ConnectWithStruct(struct CONNECT_MESSAGE *connect_message_data);

/**
 * @brief Sets the keep-alive value for connection timeouts.
 *
 * This function sets the keep-alive value for the MQTT connection timeouts. The keep-alive value specifies the maximum
 * number of seconds that can elapse between the transmission of any two packets from the client to the broker. If no
 * packets are transmitted within this period, the broker assumes that the client has disconnected and terminates the
 * connection.
 *
 * @param keepalive The keep-alive value in seconds.
 *
 * @return None.
 */
static void MQTT311_SetKeepAlive(uint16_t keepalive) 
{
    userdata.keepAlive = keepalive;
}

/**
 * @brief Appends the client ID to the byte array.
 *
 * This function appends the client ID to the byte array used in MQTT communication. The client ID identifies the device
 * that is connected to the MQTT broker and is used to maintain a persistent connection between the two.
 *
 * @param client_id The client ID to append to the byte array.
 *
 * @return None.
 */
static void MQTT311_AppendClientID(const char* client_id) 
{
    uint16_t deviceIDLength = strlen(client_id);

    MQTT311_AppendData(client_id, deviceIDLength, true);
}

/**
 * @brief Appends the username and password to the byte array.
 *
 * This function appends the username and password to the byte array used in MQTT communication. The username and password
 * are optional and can be used to provide authentication and access control to the MQTT broker.
 *
 * @return None.
 */
static void MQTT311_AppendUsernameAndPassword(void) 
{
    uint16_t usernameLength = strlen(userdata.username);
    uint16_t passwordLength = strlen(userdata.password);

    /* Append username to byte array */
    MQTT311_AppendData(userdata.username, usernameLength, true);

    /* Append password to byte array */
    MQTT311_AppendData(userdata.password, passwordLength, true);
}

/**
 * @brief Appends the will topic to the byte array.
 *
 * This function appends the will topic to the byte array used in MQTT communication. The will topic is the topic on which
 * the last will and testament message will be published in case the client unexpectedly disconnects from the MQTT broker.
 *
 * @param will_topic The will topic to append to the byte array.
 *
 * @return None.
 */
static void MQTT311_AppendWillTopic(const char* will_topic) 
{
    uint16_t willTopicLength = strlen(will_topic);

    MQTT311_AppendData(will_topic, willTopicLength, true);
}

/**
 * @brief Appends the will message to the byte array.
 *
 * This function appends the will message to the byte array used in MQTT communication. The will message is the message that
 * will be published on the will topic in case the client unexpectedly disconnects from the MQTT broker.
 *
 * @param will_message The will message to append to the byte array.
 *
 * @return None.
 */
static void MQTT311_AppendWillMessage(const char* will_message) 
{

    uint16_t willMessageLength = strlen(will_message);

    MQTT311_AppendData(will_message, willMessageLength, true);

}

/**
 * @brief Connects to a MQTT Broker using the provided username and password.
 *
 * This function establishes a connection to a MQTT Broker using the provided username and password.
 * The connection details are passed in as a pointer to a `CONNECT_MESSAGE` structure.
 *
 * @param connect_message_data Pointer to a `CONNECT_MESSAGE` structure containing the connection details.
 *
 * @return None.
 */
static void MQTT311_ConnectWithStruct(struct CONNECT_MESSAGE *connect_message_data) 
{
    /* Making sure the current_index starts at 0 */
    current_index = 0;

    /* Set keep alive */
    MQTT311_SetKeepAlive(connect_message_data->keep_alive);

    /* Appending CONNECT packet type*/
    bytes_to_send[current_index++] = connect_message_data->packet_type;

    /* Remaining size so far is 0 */
    bytes_to_send[current_index++] = connect_message_data->remaining_length;

    /* Appending protocol name and length */
    MQTT311_AppendData(connect_message_data->protocol_name, connect_message_data->protocol_name_length, true);

    /* Appending protocol version */
    bytes_to_send[current_index++] = connect_message_data->protocol_version;

    uint8_t connect_flags = (connect_message_data->_username << USERNAME_FLAG) | (connect_message_data->_password << PASSWORD_FLAG) |
                            (connect_message_data->will_retain << WILL_RETAIN_FLAG) | (connect_message_data->will_qos1 << WILL_QoS1_FLAG) |
                            (connect_message_data->will_qos2 << WILL_QoS2_FLAG) | (connect_message_data->clean_session << CLEAN_SESSION_FLAG);

    /* Appending control flags */
    bytes_to_send[current_index++] = connect_flags;

    /* Appending keep alive */
    bytes_to_send[current_index++] = connect_message_data->keep_alive >> 8;
    bytes_to_send[current_index++] = connect_message_data->keep_alive & 0xFF;

    /* Append client ID */
    MQTT311_AppendClientID(userdata.deviceID);

    /* Append will topic and message  */
    if (connect_flags & (1 << WILL_FLAG)) 
    {
        MQTT311_AppendWillTopic(connect_message_data->willTopic);
        MQTT311_AppendWillMessage(connect_message_data->willMessage);
    }

    /* Append connect payload (username, password) */
    if ((connect_flags & (1 << USERNAME_FLAG)) && (connect_flags & (1 << PASSWORD_FLAG))) 
    {
        MQTT311_AppendUsernameAndPassword();
    }

    /* Encode remaining length if larger than 127 */
    connect_message_data->remaining_length = MQTT311_CheckRemainingLength();

    /* Append remaining size */
    bytes_to_send[1] = connect_message_data->remaining_length;

    bool redelivery_flag = false;

    while(!redelivery_flag)
    {
        /* Send data to server */
        MQTT311_SendToMQTTBroker(current_index);

        /* Read the acknowledge */
        redelivery_flag = MQTT311_Connack();

        if(!redelivery_flag) 
        {
            MQTT311_Print("Unsuccesfull connection, trying to reconnect...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        else
        {
            break;
        }
    }

    /* Free the dynamically allocated structure */
    vPortFree(connect_message_data->willTopic);
    vPortFree(connect_message_data->willMessage);
    vPortFree(connect_message_data->protocol_name);
    vPortFree(connect_message_data);
}

/**
 * @brief Connects to an MQTT broker using the provided connection details.
 *
 * This function establishes a connection to an MQTT broker using the provided connection details.
 * The connection details include the connect flags, keep-alive value, will topic, and will message.
 *
 * @param connect_flags The connect flags for the connect message.
 * @param keepalive The keep-alive value in seconds.
 * @param will_topic The will topic for the last will and testament message (can be NULL if not used).
 * @param will_message The will message for the last will and testament message (can be NULL if not used).
 *
 * @return None.
 */
void MQTT311_Connect(uint8_t connect_flags, uint16_t keepalive, const char* will_topic, const char* will_message) 
{
    /* Creating a CONNECT_MESSAGE structure */
    struct CONNECT_MESSAGE* connect_message_data = (struct CONNECT_MESSAGE*)pvPortMalloc(sizeof *connect_message_data);
    NULL_CHECK(connect_message_data)

    /* Filling up the structure - General Data */
    connect_message_data->packet_type = CONNECT;
    connect_message_data->protocol_name_length = (PROTOCOL_NAME_MSB << 8) | (PROTOCOL_NAME_LSB);
    connect_message_data->protocol_name = (char*) pvPortMalloc(strlen(PROTOCOL_NAME)+1);
    NULL_CHECK(connect_message_data->protocol_name)
    memcpy(connect_message_data->protocol_name, PROTOCOL_NAME, strlen(PROTOCOL_NAME)+1);
    connect_message_data->protocol_version = PROTOCOL_VERSION;

    /* Filling up the structure - Connect Flags */
    connect_message_data->_username = connect_flags & (1 << USERNAME_FLAG);
    connect_message_data->_password = connect_flags & (1 << PASSWORD_FLAG);
    connect_message_data->will_retain = connect_flags & (1 << WILL_RETAIN_FLAG);
    connect_message_data->will_qos1 = connect_flags & (1 << WILL_QoS1_FLAG);
    connect_message_data->will_qos2 = connect_flags & (1 << WILL_QoS2_FLAG);
    connect_message_data->clean_session = connect_flags & (1 << CLEAN_SESSION_FLAG);

    /* Filling up the structure - Keep Alive */
    connect_message_data->keep_alive = keepalive;

    /* Filling up the structure - Will Topic and Will Message Data */
    connect_message_data->willTopic  = (char*) pvPortMalloc(strlen(will_topic)+1);
    NULL_CHECK(connect_message_data->willTopic)
    memcpy(connect_message_data->willTopic, will_topic, strlen(will_topic)+1);

    connect_message_data->willMessage  = (char*) pvPortMalloc(strlen(will_message)+1);
    NULL_CHECK(connect_message_data->willMessage)
    memcpy(connect_message_data->willMessage, will_message, strlen(will_message)+1);

    /* Connecting a function */
    connect_message_data->MQTT311_ConnectWithStruct = &MQTT311_ConnectWithStruct;

    /* Creating a MQTTPacket and adding CONNECT packet to the Queue */
    struct MQTTPacket *mqtt_packet = (struct MQTTPacket *) pvPortMalloc(sizeof *mqtt_packet);
    NULL_CHECK(mqtt_packet)
    mqtt_packet->parent = mqtt_packet;
    mqtt_packet->mqtt_packet_type = eCONNECT;
    mqtt_packet->packet_data.connect_message_structure = connect_message_data;

    /* Save connect flags */
    userdata.connect_flags = connect_flags;
    
    /* Send to queue for the sending task to receive */
    xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY ); 
}