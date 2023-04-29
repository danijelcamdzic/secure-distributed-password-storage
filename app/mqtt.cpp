#include "mqtt.h"
#include "rsa.h"

const std::string SERVER_ADDRESS("tcp://mqtt.eclipseprojects.io:1883");
const std::string CLIENT_ID("MQTT_CPP_Subscriber");
const std::string SUB_TOPIC("/topic/topic1");
const std::string PUB_TOPIC("/topic/topic3");

void callback::message_arrived(mqtt::const_message_ptr msg)
{
    std::cout << "Message arrived: " << msg->get_topic() << ": " << msg->to_string() << std::endl;
}

void publisher(mqtt::async_client& client, const std::string& public_key_filename)
{
    std::string input;
    while (true)
    {
        std::getline(std::cin, input);
        if (!input.empty())
        {
            std::string encrypted_msg = encrypt_message(input, public_key_filename);
            if (!encrypted_msg.empty())
            {
                // Append "END_MESSAGE" to the encrypted message
                encrypted_msg += "END_MESSAGE";

                auto msg = mqtt::make_message(PUB_TOPIC, encrypted_msg);
                msg->set_qos(1);
                client.publish(msg)->wait_for(std::chrono::seconds(10));
                std::cout << "Encrypted message published: " << input << std::endl;
            }
        }
    }
}