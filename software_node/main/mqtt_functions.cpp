/**
 * @file mqtt_functions.cpp
 * @brief Contains necessary defines, variables definitions
 * and function definitions for using the paho mqtt c++ library
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include "mqtt_functions.h"

/* Variable for general broker connection */
const std::string SERVER_ADDRESS("tcp://mqtt.eclipseprojects.io:1883");
const std::string CLIENT_ID("access_node");
const std::string SERVER_CERTIFICATE_PATH("../mqtt_ecplipseprojects_io_certificate.pem");    /**< Can be empty if only TCP connection will be used*/

/* Variable for commanding the hardware nodes */
const std::string RETRIEVE_PASSWORD_COMMAND("GetPassEND_MESSAGE");          /**< Used in the restore password command */
const std::string END_MESSAGE_FLAG("END_MESSAGE");                          /**< Sent at the end of every encrypted message to hardware nodes */

/* Variable for communication with hardware nodes */
const std::string TOPIC_SUB_HW_1("/topic/sub/hw_node_1");                   /**< On this topic the hardware node 1 sends messages */
const std::string TOPIC_PUB_HW_1("/topic/pub/hw_node_1");                   /**< On this topic the hardware node 1 listens for messages */
/* ... Add more topics for subscription and publishing (one for each hardware node) */
const std::string TOPIC_PUB_ALL("/topic/pub/all");                          /**< On this topic all hardware nodes are listening */

/* Vectors for containing the publish and subscribe topics */
const std::vector<std::string> sub_topics = {TOPIC_SUB_HW_1};               /**< Add subscription topics for each hardware node in the vector */
const std::vector<std::string> pub_topics = {TOPIC_PUB_HW_1};               /**< Add publishing topics for each hardware node in the vector */

/* Variables for connection to MQTT broker with Paho MQTT C++ library */
mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
static mqtt::connect_options connOpts;

/* Callback function */
callback mqttCallbackFunction;

/* ------------------------- EXPLANATION ------------------------------------ */

/**
 * The message_arrived function is a callback that gets invoked when an MQTT message is received.
 * It stores the received messages in the received_messages buffer and uses a mutex (received_messages_mutex)
 * to ensure thread-safe access to the buffer. After adding a message to the buffer, the function notifies
 * the condition variable (received_messages_cv) to signal that a new message has arrived.
 *
 * The wait_for_messages function uses the same mutex (received_messages_mutex) and condition variable
 * (received_messages_cv) to block and wait until the specified number of unique topics have been received.
 * When the condition variable is notified by the message_arrived function, wait_for_messages checks if
 * the required number of unique topics has been reached. If so, it unblocks and returns. Otherwise, it continues
 * to wait for more messages.
 *
 * The get_received_messages function is used to retrieve the received messages from the received_messages
 * buffer in a thread-safe manner. It locks the same mutex (received_messages_mutex) used by the other two
 * functions to ensure exclusive access to the buffer while it returns a copy of the received messages.
 * 
 * The purpose of the mutex in the context of the wait_for_messages, get_received_messages, and message_arrived 
 * functions is to ensure thread-safe access to the shared resource, which is the received_messages buffer. It does 
 * not stop a function from returning or re-entering itself. Instead, it ensures that only one thread can access the 
 * shared resource at a time. 
 * 
 * The std::unique_lock used in these functions follows the RAII pattern, which means that the lock will be automatically 
 * released when the unique_lock object goes out of scope. In the case of the wait_for_messages function, when it is 
 * waiting on the condition variable, the mutex is *temporarily released*, allowing other functions (such as message_arrived) 
 * to lock the mutex and modify the shared resource. Once the condition variable is signaled (i.e., a new message has arrived), 
 * the wait_for_messages function re-acquires the lock before checking the condition and potentially returning.
 * 
 * The purpose of locking the mutex before waiting on the condition variable is to ensure that the check of the condition and 
 * the actual wait are performed atomically. The received_messages_cv.wait(lock, ...) function internally releases the mutex 
 * while waiting, allowing other functions, such as message_arrived, to lock the mutex and access the shared resource. When the 
 * condition variable is signaled, the wait function re-acquires the mutex before returning. This ensures that when the 
 * predicate (the condition that must be met) is checked, no other function can interfere and modify the shared resource.
 * 
 * This behavior allows message_arrived to be called and lock the mutex while wait_for_messages is waiting for messages, 
 * ensuring that the shared resource is accessed safely by both functions without causing a deadlock.
 *
 * In summary, these three functions work together to handle incoming MQTT messages and provide thread-safe
 * access to the received messages buffer, using a mutex for synchronization and a condition variable for
 * efficient waiting.
 */

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Callback function that gets invoked when a message arrives via MQTT
 * @param[in] msg The received MQTT message as a constant pointer to an mqtt::const_message object
 *
 * This function overrides the message_arrived function from the Paho MQTT C++ library
 * to handle incoming MQTT messages. It stores the received messages in a buffer
 * and notifies other threads waiting for messages via a condition variable.
 */
void callback::message_arrived(mqtt::const_message_ptr msg)
{
#ifdef DEBUG
    /* Print the received messages to the console upon reception */
    std::cout << "Message arrived: " << msg->get_topic() << ": " << msg->to_string() << std::endl;
#endif

    /* Lock the mutex to ensure thread-safety when accessing the received_messages buffer */
    std::unique_lock<std::mutex> lock(received_messages_mutex);

    /* Add the received message to the buffer as a tuple containing the topic and payload */
    received_messages.emplace_back(msg->get_topic(), msg->to_string());

    /* Unlock the mutex */
    lock.unlock();

    /* Notify any waiting threads that a new message has arrived */
    received_messages_cv.notify_one();
}

/**
 * @brief Retrieve the received MQTT messages stored in the buffer
 * @return A vector of pairs containing the received messages' topics and payloads
 *
 * This function provides thread-safe access to the received_messages buffer,
 * which contains MQTT messages received by the message_arrived callback function.
 */
std::vector<std::pair<std::string, std::string>> callback::get_received_messages() const
{
    /* Lock the mutex to ensure thread-safety when accessing the received_messages buffer */
    std::unique_lock<std::mutex> lock(received_messages_mutex);

    /* Return a copy of the received_messages buffer */
    return received_messages;

    /* The lock will be automatically released when the function returns, as std::unique_lock follows RAII principles */
}

/**
 * @brief Wait for a specified number of unique topics to be received before returning or until the timeout expires
 * @param[in] num_unique_topics The number of unique topics to wait for before the function returns
 * @param[in] timeout_duration The duration (in milliseconds) to wait before the function times out
 *
 * This function blocks and waits until the specified number of unique topics have been
 * received via the message_arrived callback function, or the timeout expires. It uses
 * a condition variable to efficiently wait for the arrival of new messages without
 * busy-waiting or polling.
 */
void callback::wait_for_messages(uint32_t num_unique_topics, uint32_t timeout_duration)
{
    /* Announce that wait for messages is in process */
    std::cout << "Waiting for messages from the hardware nodes..." << std::endl;

    /* Lock the mutex to ensure thread-safety when accessing the received_messages buffer */
    std::unique_lock<std::mutex> lock(received_messages_mutex);

    /* Wait on the condition variable with a timeout */
    bool received_enough_messages = received_messages_cv.wait_for(lock, std::chrono::milliseconds(timeout_duration), [this, num_unique_topics] {
        /* Create a set to store unique topics from the received_messages buffer */
        std::unordered_set<std::string> unique_topics;

        /* Iterate over the received_messages buffer and insert topics into the unique_topics set */
        for (const auto& [topic, message] : received_messages) {
            unique_topics.insert(topic);
        }

        /* Check if the size of the unique_topics set is greater or equal to the specified number of unique topics */
        return unique_topics.size() >= num_unique_topics;
    });

    /* Check if enough unique topics were received before the timeout */
    if (!received_enough_messages) {
        /* Create a set to store unique topics from the received_messages buffer */
        std::unordered_set<std::string> unique_topics;

        /* Iterate over the received_messages buffer and insert topics into the unique_topics set */
        for (const auto& [topic, message] : received_messages) {
            unique_topics.insert(topic);
        }

        /* Throw an error with the required information */
        throw std::runtime_error("Timeout expired while waiting for all messages. Received " +
                                 std::to_string(unique_topics.size()) + " messages, but expected " +
                                 std::to_string(num_unique_topics) + ".");
    }

    /* The lock will be automatically released when the function returns, as std::unique_lock follows RAII principles */
}

/**
 * @brief Connect to the MQTT broker with TCP/SSL and set the callback function to receive the messages
 * @param[in] username The username for connecting to the MQTT broker
 * @param[in] password The password for connecting to the MQTT broker
 * @param[in] certificatePath The path to the certificate file for TCP/SSL connection
 *
 * This function sets the callback function which receives the MQTT messages and does further processing.
 * It also connects to the MQTT broker using the connOpts options and provided username, password, and (optinally)
 * certificate file.
 */
void mqtt_connect(const std::string& username, const std::string& password, const std::string& certificatePath)
{
    /* Set the callback function */
    client.set_callback(mqttCallbackFunction);

    /* Set the keepalive interval and clean session */
    connOpts.set_keep_alive_interval(120);
    connOpts.set_clean_session(true);

    /* Set the username and password for the connection */
    connOpts.set_user_name(username.c_str());
    connOpts.set_password(password.c_str());

    /* Check whether to connect with TCP or SSL */
    if (certificatePath != "") {
        /* Set the SSL/TLS options */
        mqtt::ssl_options sslOpts;
        sslOpts.set_trust_store(certificatePath);
        connOpts.set_ssl(sslOpts);

        /* Announce connection with SSL */
        std::cout << "Connecting to the MQTT server with SSL..." << std::endl;
    }
    else {
        /* Announce connection with TCP */
        std::cout << "Connecting to the MQTT server with TCP..." << std::endl;
    }

    /* Connect to the MQTT server using the connOpts */
    client.connect(connOpts)->wait();
    std::cout << "Connected successfully!" << std::endl;
}

/**
 * @brief Subscribes to a MQTT topic
 * @param[in] topic MQTT topic to subscribe to
 *
 * This function subscribes to a given MQTT topic.
 */
void mqtt_subscribe(const std::string& topic)
{
    try 
    {
        /* Subscribe to a MQTT topic */
        client.subscribe(topic, 1)->wait();
        std::cout << "Subscribed to topic: " << topic << std::endl;
    }
    catch (const mqtt::exception& exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
}

/**
 * @brief Publishes message to an MQTT topic
 * @param[in] topic MQTT topic to send message to
 * @param[in] message vector of unsigned chars that represent a message to send
 *
 * This function publishes a message to a given MQTT topic.
 */
void mqtt_publish(const std::string& topic, const std::vector<unsigned char>& message)
{
    try
    {
        /* Create a message from the given vector of unsigned chars that represent the message to publish */
        auto msg = mqtt::make_message(topic, std::string(message.begin(), message.end()));
        msg->set_qos(1);
        /* Publish the message */
        client.publish(msg)->wait_for(std::chrono::seconds(10));
        std::cout << "Message published on topic: " << topic << std::endl;

#ifdef DEBUG
        /* Print the published messages to the console upon send */
        std::cout << "Sent message: " << std::string(message.begin(), message.end()) << std::endl;
#endif
    }
    catch (const mqtt::exception& exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
}
