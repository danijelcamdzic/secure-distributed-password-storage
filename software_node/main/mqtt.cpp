#include "mqtt.h"

const std::string SERVER_ADDRESS("tcp://mqtt.eclipseprojects.io:1883");
const std::string CLIENT_ID("access_node");
const std::string TOPIC_SUB_HW_1("/topic/sub/hw_node_1");
const std::string TOPIC_PUB_HW_1("/topic/pub/hw_node_1");
const std::string TOPIC_PUB_ALL("/topic/pub/all");
const std::vector<std::string> sub_topics = {TOPIC_SUB_HW_1};
const std::vector<std::string> pub_topics = {TOPIC_PUB_HW_1};

mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
static mqtt::connect_options connOpts;
callback mqttCallbackFunction;

void callback::message_arrived(mqtt::const_message_ptr msg)
{
    std::cout << "Message arrived: " << msg->get_topic() << ": " << msg->to_string() << std::endl;
    std::unique_lock<std::mutex> lock(received_messages_mutex);
    received_messages.emplace_back(msg->get_topic(), msg->to_string());
    lock.unlock();
    received_messages_cv.notify_one();
}

std::vector<std::pair<std::string, std::string>> callback::get_received_messages() const
{
    std::unique_lock<std::mutex> lock(received_messages_mutex);
    return received_messages;
}

void callback::wait_for_messages(int num_messages)
{
    std::unique_lock<std::mutex> lock(received_messages_mutex);
    received_messages_cv.wait(lock, [this, num_messages] { return received_messages.size() >= num_messages; });
}

// void callback::wait_for_messages(int num_unique_topics)
// {
//     std::unique_lock<std::mutex> lock(received_messages_mutex);
//     received_messages_cv.wait(lock, [this, num_unique_topics] { 
//         std::unordered_set<std::string> unique_topics;
//         for (const auto& [topic, message] : received_messages) {
//             unique_topics.insert(topic);
//         }
//         return unique_topics.size() >= num_unique_topics; 
//     });
// }

void mqtt_connect(void)
{
    client.set_callback(mqttCallbackFunction);
    connOpts.set_keep_alive_interval(120);
    connOpts.set_clean_session(true);

    std::cout << "Connecting to the MQTT server..." << std::endl;
    client.connect(connOpts)->wait();
    std::cout << "Connected successfully!" << std::endl;
}

void mqtt_subscribe(const std::string& topic)
{
    try 
    {
        std::cout << "Subscribing to topic: " << topic << std::endl;
        client.subscribe(topic, 1)->wait();
        std::cout << "Subscribed!" << std::endl;
    }
    catch (const mqtt::exception& exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
}

void mqtt_publish(const std::string& topic, const std::string& message)
{
    try
    {
        auto msg = mqtt::make_message(topic, message);
        msg->set_qos(1);
        client.publish(msg)->wait_for(std::chrono::seconds(10));
        std::cout << "Message published: " << message << std::endl;
    }
    catch (const mqtt::exception& exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
}