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
    /* Declare a string variable to store the user's password */
    std::string password;

    /* Prompt the user to enter their password */
    std::cout << "Enter your password: ";

#if defined(_WIN32) || defined(_WIN64)
    /* Get the standard input handle for Windows systems */
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;

    /* Save the current console mode */
    GetConsoleMode(hStdin, &mode);

    /* Set console mode to disable echo input (hiding the typed password) */
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    /* Read the user's password from input */
    std::getline(std::cin, password);

    /* Restore the original console mode */
    SetConsoleMode(hStdin, mode);
#else
    /* Declare termios structs for saving and modifying terminal attributes on Unix systems */
    termios oldt, newt;

    /* Get the current terminal attributes */
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    /* Modify the terminal attributes to disable echo */
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    /* Read the user's password from input */
    std::getline(std::cin, password);

    /* Restore the original terminal attributes */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    /* Print a newline character to separate the output from the password prompt */
    std::cout << std::endl;

    /* Return the entered password */
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

    /* Connect to the MQTT broker */
    mqtt_connect();
    /* Subscribe to all topics from the list */
    for (const std::string& topic : sub_topics) {
        mqtt_subscribe(topic);
    }

    /* Save password taken from the console */
    if (option == "save_password") {
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

        /* Encrypt and send the shares */
        std::vector<std::string> encrypted_shares(SHAMIR_NUM_SHARES);
        for (size_t i = 0; i < SHAMIR_NUM_SHARES; i++) {
            std::string share_data(reinterpret_cast<char*>(shares[i]), sss_SHARE_LEN);
            encrypted_shares[i] = rsa_encrypt_message(share_data, public_keys_hw_nodes[i]);
            encrypted_shares[i] += END_MESSAGE_FLAG;
            mqtt_publish(pub_topics[i], encrypted_shares[i]);
        }
    /* Retrieve password from the hw nodes */
    } else if (option == "get_password") {
        /* Send command to retrieve the password from hw nodes */
        mqtt_publish(TOPIC_PUB_ALL, RETRIEVE_PASSWORD_COMMAND);

        /* Wait for the minimum number of messages */
        mqttCallbackFunction.wait_for_messages(SHAMIR_THRESHOLD);

        /* Print the messages for debugging */
        auto received_messages = mqttCallbackFunction.get_received_messages();
        for (const auto& [topic, message] : received_messages) {
            std::cout << "Received message from topic " << topic << ": " << message << std::endl;
        }

        /* 1. Decrypt the messages using rsa_decrypt_message */
        std::vector<std::string> decrypted_share_strings;
        for (const auto& [topic, message] : received_messages) {
            std::string decrypted_message = rsa_decrypt_message(message, RSA_PRIVATE_KEY);
            decrypted_share_strings.push_back(decrypted_message);
        }

        /* 2. Convert std::string to sss_Share and create a std::vector<sss_Share> */
        std::vector<sss_Share> shares(decrypted_share_strings.size());
        for (size_t i = 0; i < decrypted_share_strings.size(); ++i) {
            if (decrypted_share_strings[i].size() > sss_SHARE_LEN) {
                throw std::runtime_error("Decrypted share string length must be less than or equal to sss_SHARE_LEN.");
            }
            memcpy(shares[i], decrypted_share_strings[i].data(), decrypted_share_strings[i].size());
        }

        /* 3. Call the sss_recombine_password_from_shares function */
        std::string restored_password = sss_recombine_password_from_shares(shares);
        std::cout << "Restored password is: " << restored_password << std::endl;
        
    } else {
        std::cerr << "Invalid option. Please provide either 'save_password' or 'get_password' as an argument." << std::endl;
    }

    return 0;
}
