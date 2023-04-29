#ifndef MQTT_H
#define MQTT_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include "mqtt/async_client.h"

extern const std::string SERVER_ADDRESS;
extern const std::string CLIENT_ID;
extern const std::string SUB_TOPIC;
extern const std::string PUB_TOPIC;

class callback : public virtual mqtt::callback
{
public:
    void message_arrived(mqtt::const_message_ptr msg) override;
};

void publisher(mqtt::async_client& client, const std::string& public_key_filename);

#endif