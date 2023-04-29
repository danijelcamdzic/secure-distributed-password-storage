#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include "mqtt/async_client.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "sss.h"
#include "randombytes.h"

const std::string SERVER_ADDRESS("tcp://mqtt.eclipseprojects.io:1883");
const std::string CLIENT_ID("MQTT_CPP_Subscriber");
const std::string SUB_TOPIC("/topic/topic1");
const std::string PUB_TOPIC("/topic/topic3");

class callback : public virtual mqtt::callback
{
    void message_arrived(mqtt::const_message_ptr msg) override
    {
        std::cout << "Message arrived: " << msg->get_topic() << ": " << msg->to_string() << std::endl;
    }
};

std::string encrypt_message(const std::string& message, const std::string& public_key_filename)
{
    std::ifstream pub_key_file(public_key_filename);
    if (!pub_key_file)
    {
        std::cerr << "Error opening public key file: " << public_key_filename << std::endl;
        return "";
    }

    std::string pub_key_str((std::istreambuf_iterator<char>(pub_key_file)), std::istreambuf_iterator<char>());
    BIO* bio = BIO_new_mem_buf(pub_key_str.data(), pub_key_str.size());
    RSA* rsa_pubkey = RSA_new();

    if (!PEM_read_bio_RSA_PUBKEY(bio, &rsa_pubkey, nullptr, nullptr))
    {
        std::cerr << "Error reading public key from BIO" << std::endl;
        BIO_free(bio);
        return "";
    }

    int rsa_size = RSA_size(rsa_pubkey);
    std::vector<unsigned char> encrypted_message(rsa_size);

    int encrypt_len = RSA_public_encrypt(message.size(), reinterpret_cast<const unsigned char*>(message.data()), encrypted_message.data(), rsa_pubkey, RSA_PKCS1_PADDING);
    if (encrypt_len == -1)
    {
        std::cerr << "Error encrypting message: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        RSA_free(rsa_pubkey);
        BIO_free(bio);
        return "";
    }

    RSA_free(rsa_pubkey);
    BIO_free(bio);
    return std::string(reinterpret_cast<char*>(encrypted_message.data()), encrypt_len);
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

    uint8_t data[sss_MLEN], restored[sss_MLEN];
	sss_Share shares[5];
	size_t idx;
	int tmp;

	// Read a message to be shared
	strncpy(reinterpret_cast<char*>(data), "Tyler Durden isn't real.", sizeof(data));

	// Split the secret into 5 shares (with a recombination theshold of 4)
	sss_create_shares(shares, data, 5, 4);

	// Combine some of the shares to restore the original secret
	tmp = sss_combine_shares(restored, shares, 5);

    /* Print the messages */
    std::cout << "Original message" << std::endl;
    std::cout << data << std::endl;
    std::cout << "Restored message:" << std::endl;
    std::cout << restored << std::endl;

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

