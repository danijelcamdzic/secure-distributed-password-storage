#ifndef MQTT_H
#define MQTT_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "mqtt/callback.h"
#include "mqtt/async_client.h"

#define RETRIEVE_PASSWORD_COMMAND   "GetPassEND_MESSAGE"
#define END_MESSAGE_FLAG            "END_MESSAGE"

extern const std::string SERVER_ADDRESS;
extern const std::string CLIENT_ID;
extern const std::string TOPIC_SUB_HW_1;
extern const std::string TOPIC_PUB_HW_1;
extern const std::string TOPIC_PUB_ALL;
extern const std::vector<std::string> sub_topics;
extern const std::vector<std::string> pub_topics;

class callback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override;
    std::vector<std::pair<std::string, std::string>> get_received_messages() const;
    void wait_for_messages(int num_messages);

private:
    std::vector<std::pair<std::string, std::string>> received_messages;
    mutable std::mutex received_messages_mutex;
    std::condition_variable received_messages_cv;
};
extern callback mqttCallbackFunction;

void mqtt_connect(void);
void mqtt_subscribe(const std::string& topic);
void mqtt_publish(const std::string& topic, const std::string& message);

#endif