/**
 * @file mqtt_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the paho mqtt c++ library
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#ifndef MQTT_FUNCTIONS_H
#define MQTT_FUNCTIONS_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <mutex>
#include <unordered_set>
#include <condition_variable>
#include <chrono>

/* Paho MQTT C++ library */
#include "mqtt/callback.h"
#include "mqtt/async_client.h"

/* Wait for 15s for message confirmations or passphrase pieces */
#define WAIT_PERIOD_MS   30000

/* Variable declarations for commanding the hardware nodes */
extern const std::string RETRIEVE_PASSWORD_COMMAND;             /**< Used in the restore password command */
extern const std::string END_MESSAGE_FLAG;                      /**< Sent at the end of every encrypted message to hardware nodes */

/* Variable declarations for general broker connection */
extern const std::string SERVER_ADDRESS;
extern const std::string CLIENT_ID;

/* Variable declarations for communication with hardware nodes */
extern const std::string TOPIC_SUB_HW_1;                        /**< On this topic the hardware node 1 sends messages */
extern const std::string TOPIC_PUB_HW_1;                        /**< On this topic the hardware node 1 listens for messages */
extern const std::string TOPIC_PUB_ALL;                         /**< On this topic all hardware nodes are listening */

/* Vectors declarations for containing the publish and subscribe topics */
extern const std::vector<std::string> sub_topics;
extern const std::vector<std::string> pub_topics;

/* Class declaration for overriding Paho MQTT C++ functions */
class callback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override;                         /**< Function for receiving messages */
    std::vector<std::pair<std::string, std::string>> get_received_messages() const;     /**< Function which returns the received messages */
    void wait_for_messages(uint32_t num_unique_topics, uint32_t timeout_duration);      /**< Function which wait for num_messages from different topics */

private:
    std::vector<std::pair<std::string, std::string>> received_messages;                 /**< Received messages structure */
    mutable std::mutex received_messages_mutex;                                         /**< Mutex for atomic operation ensurance */
    std::condition_variable received_messages_cv;                                       /**< Variable for notification or done operation */
};

/* Callback function declaration */
extern callback mqttCallbackFunction;

/* Function prototypes */
void mqtt_connect(const std::string& username, const std::string& password);
void mqtt_subscribe(const std::string& topic);
void mqtt_publish(const std::string& topic, const std::vector<unsigned char>& message);

#endif /* MQTT_FUNCTIONS_H */