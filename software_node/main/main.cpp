#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "mqtt.h"
#include "rsa.h"
#include "shamir_secret_sharing.h"

std::string read_password() 
{
    std::string password;
    std::cout << "Enter your password: ";

#if defined(_WIN32) || defined(_WIN64)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    std::getline(std::cin, password);

    SetConsoleMode(hStdin, mode);
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    std::cout << std::endl;

    return password;
}

int main(int argc, char *argv[]) 
{
    /* Read the arguments to deduce whether to save or retrieve password */
    if (argc != 2) {
        /* Help user understand the usage */
        std::cerr << "Usage: " << argv[0] << " [save_password|get_password]" << std::endl;
        return 1;
    }
    std::string option = argv[1];

    /* Save password taken from the console */
    if (option == "save_password") {
        /* Connect to the MQTT broker */
        mqtt_connect();
        /* Subscribe to all topics */
        for (const std::string& topic : sub_topics) {
            mqtt_subscribe(topic);
        }

        /* Read the password from the console */
        std::string password = read_password();
        std::cout << "Entered password: " << password << std::endl;

        /* Split the password using Shamir's secret sharing formula */
        std::vector<sss_Share> shares(SHAMIR_NUM_SHARES);
        sss_split_password_into_shares(password, shares);

        /* Check if the number of shares match the number of publish topics (HW nodes) */
        if (SHAMIR_NUM_SHARES != pub_topics.size()) {
            throw std::runtime_error("Number of shares must match the number of publish topics!");
        }

        // Encrypt and send the shares
        std::vector<std::string> encrypted_shares(SHAMIR_NUM_SHARES);
        for (size_t i = 0; i < SHAMIR_NUM_SHARES; i++) {
            std::string share_data(reinterpret_cast<char*>(shares[i]), sss_SHARE_LEN);
            encrypted_shares[i] = rsa_encrypt_message(share_data, public_keys_hw_nodes[i]);
            encrypted_shares[i] += "END_MESSAGE";
            mqtt_publish(pub_topics[i], encrypted_shares[i]);
        }
    } else if (option == "get_password") {
        mqtt_publish(TOPIC_PUB_ALL, RETRIEVE_PASSWORD);
        // Wait for messages from the subscribed topics
        // auto received_messages = wait_for_messages(cb, num_messages_to_wait_for, sub_topics);

        // Process the received messages
        // for (const auto& [topic, message] : received_messages)
        // {
        //     std::cout << "Received message from topic: " << topic << ", content: " << message << std::endl;
        // }

    } else {
        std::cerr << "Invalid option. Please provide either 'save_password' or 'get_password' as an argument." << std::endl;
    }

    return 0;
}
