/***********************************************************************
* FILENAME:        MQTT311_Connect.c             
*
* DESCRIPTION:
*                  Contains variables and function definitions for the 
*                  MQTT 3.1.1 CONNECT package.
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

/* Private function declarations */
static void MQTT311_SetKeepAlive(uint16_t keepalive);
static void MQTT311_AppendClientID(const char* client_id);
static void MQTT311_AppendUsernameAndPassword(void);
static void MQTT311_AppendWillTopic(const char* will_topic);
static void MQTT311_AppendWillMessage(const char* will_message);
static void MQTT311_ConnectWithStruct(struct CONNECT_MESSAGE *connect_message_data);

/*
 * Function: MQTT311_SetKeepAlive
 * ----------------------------
 *   Sets the keep alive value for connection timeouts.
 *
 *   keepalive: value after which the client disconnects if no messages
 *              have been transmitted.
 *
 *   returns: no return value
 */
static void MQTT311_SetKeepAlive(uint16_t keepalive) 
{
    userdata.keepAlive = keepalive;
}

/*
 * Function: MQTT311_AppendClientID
 * ----------------------------
 *   Appends client ID to the byte array.
 *
 *   client_id: id of the device
 *
 *   returns: no return value
 */
static void MQTT311_AppendClientID(const char* client_id) 
{

    uint16_t deviceIDLength = strlen(client_id);

    MQTT311_AppendData(client_id, deviceIDLength, true);

}

/*
 * Function: MQTT311_AppendUsernameAndPassword
 * ----------------------------
 *   Appends username and password to the byte array.
 *
 *   returns: no return value
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

/*
 * Function: MQTT311_AppendWillTopic
 * ----------------------------
 *   Appends will topic to the byte array
 *
 *   will_topic: will topic of the connect message
 *
 *   returns: no return value
 */
static void MQTT311_AppendWillTopic(const char* will_topic) 
{
    uint16_t willTopicLength = strlen(will_topic);

    MQTT311_AppendData(will_topic, willTopicLength, true);

}

/*
 * Function: MQTT311_AppendWillMessage
 * ----------------------------
 *   Appends will message to the byte array
 *
 *   will_message: will message of the connect message
 *
 *   returns: no return value
 */
static void MQTT311_AppendWillMessage(const char* will_message) 
{

    uint16_t willMessageLength = strlen(will_message);

    MQTT311_AppendData(will_message, willMessageLength, true);

}

/*
 * Function: MQTT311_ConnectWithStruct
 * ----------------------------
 *   Connects to Cumulocity tenant by providing user's username and password
 *
 *   connect_message_data: connect message structure 
 *
 *   returns: no return value
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

        redelivery_flag = true; // DUMMY CODE

        /* Read the acknowledge and if no acknowledge, try to reconnect */
        // redelivery_flag = get_connect_acknowledgement();

        // if(!redelivery_flag) 
        // {
        //     printf("\r\nUnsuccesfull connection, trying to reconnect..\r\n");
        //     msleep(1000);
        // }
        // else
        // {
        //     break;
        // }
    }
    /* Free the dynamically allocated structure */
    vPortFree(connect_message_data->willTopic);
    vPortFree(connect_message_data->willMessage);
    vPortFree(connect_message_data->protocol_name);
    vPortFree(connect_message_data);
}

/*
 * Function: MQTT311_Connect
 * ----------------------------
 *   Connects to Cumulocity tenant by providing user's username and password
 *
 *   connect_flags: value of connect flags for connect message
 *   keep_alive: value after which the client disconnects if no messages
 *              have been transmitted. 
 *   will_topic: message sent depending on the flag value
 *   will_message: message sent depending on the flag value 
 *
 *   returns: no return value
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
    // xQueueSend( xMQTTQueue, mqtt_packet, portMAX_DELAY ); 

    MQTT311_ConnectWithStruct(connect_message_data);
}