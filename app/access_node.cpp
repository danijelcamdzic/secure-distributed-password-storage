#include <iostream>
#include <thread>
#include <fstream>
#include <vector>

#include "mqtt.h"
#include "rsa.h"
#include "shamir_secret_sharing.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <public_key.pem>" << std::endl;
        return 1;
    }

    std::string public_key_filename = argv[1];

    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    callback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);

    shamir_secret_sharing_test();

    try
    {
        std::cout << "Connecting to the MQTT server..." << std::endl;
        client.connect(connOpts)->wait();
        std::cout << "Connected successfully!" << std::endl;

        std::cout << "Subscribing to topic: " << SUB_TOPIC << std::endl;
        client.subscribe(SUB_TOPIC, 1)->wait();
        std::cout << "Subscribed!" << std::endl;

        std::cout << "Type a message and press Enter to publish it on topic: " << PUB_TOPIC << std::endl;
        std::thread pub_thread(publisher, std::ref(client), public_key_filename);
        pub_thread.join();
    }
    catch (const mqtt::exception& exc)
    {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}

