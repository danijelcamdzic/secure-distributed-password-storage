/**
 * @file main.cpp
 * @brief Includes the control access to save password or get password
 * from the hardware nodes that are connected to the same MQTT broker
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */


#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

/* Libraries for secure password retrieval from console */
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

/* Header files containing utility functions */
#include "mqtt_functions.h"
#include "rsa_functions.h"
#include "shamir_secret_sharing_functions.h"


/**
 * @brief Reads a password from the user without displaying the typed characters.
 * 
 * This function prompts the user to enter their password and reads the input
 * while hiding the typed characters. It handles both Windows and Unix systems
 * by using platform-specific methods to disable character echo.
 *
 * @return The entered password as a std::string.
 */
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

/**
 * @brief The main function of the secure password storage and retrieval application.
 * 
 * This function reads command-line arguments to determine whether to save or retrieve a password.
 * It connects to the MQTT broker, subscribes to topics, and handles the saving and retrieval process.
 * When saving, it splits the password using Shamir's secret sharing, encrypts the shares, and sends
 * them to the hardware nodes. It waits confirmation that the password shares have been saved. When 
 * retrieving, it sends a command to retrieve the password, waits for the minimum number of messages, 
 * decrypts the shares, and recombines them to restore the password.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return 0 on successful execution, non-zero otherwise.
 */
int main(int argc, char *argv[]) 
{
    /* Read the arguments to deduce whether to save or retrieve password */
    if (argc != 2) {
        /* Help user understand the usage */
        std::cerr << "Usage: " << argv[0] << " [save_password|get_password]" << std::endl;
        return 1;
    }

    /* Get the command */
    std::string option = argv[1];

    /* Check to see if the options are correct */
    if ((option != "save_password") && (option != "get_password")) {
        /* Help user understand the usage */
        std::cerr << "Usage: " << argv[0] << " [save_password|get_password]" << std::endl;
        return 1;
    }

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
        std::vector<std::vector<unsigned char>> encrypted_shares(SHAMIR_NUM_SHARES);
        for (size_t i = 0; i < SHAMIR_NUM_SHARES; i++) {
            std::vector<unsigned char> share_data(shares[i], shares[i] + sss_SHARE_LEN);

            /* Uncomment to see the length of the original share */
            // std::cout << "The original share has the length of: " << share_data.size() << std::endl;

            /* Uncomment to print the share in hex */
            // std::stringstream hex_ss;
            // for (unsigned char c : share_data) {
            //     hex_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            // }
            // std::cout << "Share in hex: " << hex_ss.str() << std::endl;

            /* Encrypt share with the correct public key of the hardware node */
            encrypted_shares[i] = rsa_encrypt_message(share_data, public_keys_hw_nodes[i]);

            /* Append the END_MESSAGE_FLAG */
            std::vector<unsigned char> end_message_flag_vec(END_MESSAGE_FLAG.begin(), END_MESSAGE_FLAG.end());
            encrypted_shares[i].insert(encrypted_shares[i].end(), end_message_flag_vec.begin(), end_message_flag_vec.end());

            /* Publish the share to the correct topic of the hardware node */
            mqtt_publish(pub_topics[i], encrypted_shares[i]);
        }

        /* Wait for confirmation of reception from the hardware nodes (wait for minimum threshold confirmations) */
        mqttCallbackFunction.wait_for_messages(SHAMIR_THRESHOLD);

        /** Check if the receive messages are OK
        *   This block of code functions properly because the only topics this app is subscribing to are the ones
        *   from which the hardware nodes are sending data. No other topics should be subscribed.
        *   If other topics are subscribed to, this part will not function and the wait_for_messages function should be
        *   edited to wait only for topics that are concerning the hardware nodes communication.
        */
        auto received_messages = mqttCallbackFunction.get_received_messages();
        for (const auto& [topic, message] : received_messages) {
            /* Check if the topic is in the sub_topics vector */
            auto it = std::find(sub_topics.begin(), sub_topics.end(), topic);
            bool topic_exists = it != sub_topics.end();
            
            /* Check if the message is "OK" */
            bool message_ok = ((message[0] == 'O') && (message[1] == 'K'));
            
            if (!topic_exists || !message_ok) {
                throw std::runtime_error("The reception of the shares from the hardware node side encountered and error");
            }
        }
        /* Notify correct password saving */
        std::cout << "The shares have been sent and confirmed by the hardware nodes" << std::endl;
    /* Retrieve password from the hardware nodes */
    } else {
        /* Send command to retrieve the password from hardware nodes */
        std::vector<unsigned char> retrieve_message_command(RETRIEVE_PASSWORD_COMMAND.begin(), RETRIEVE_PASSWORD_COMMAND.end());
        mqtt_publish(TOPIC_PUB_ALL, retrieve_message_command);

        /* Wait for the minimum number of messages */
        mqttCallbackFunction.wait_for_messages(SHAMIR_THRESHOLD);

        /* Print the messages for debugging */
        auto received_messages = mqttCallbackFunction.get_received_messages();
        for (const auto& [topic, message] : received_messages) {
            // std::cout << "Received message from topic " << topic << ": " << message << std::endl;
            std::stringstream hex_ss;
            for (unsigned char c : message) {
                hex_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            }
            std::cout << "Received message from topic " << topic << ": " << hex_ss.str() << std::endl;
        }

        /* 1. Decrypt the messages using rsa_decrypt_message */
        std::vector<std::vector<unsigned char>> decrypted_shares;
        for (const auto& [topic, message] : received_messages) {
            std::cout << "Received message has length of: " << message.size() << " bytes" << std::endl;
            std::vector<unsigned char> encrypted_message(message.begin(), message.end());

            /* Decrypt with the private key of the master device */
            std::vector<unsigned char> decrypted_message_vec = rsa_decrypt_message(encrypted_message, RSA_PRIVATE_KEY);

            std::cout << "The decrypted share has the length of: " << decrypted_message_vec.size() << " bytes" << std::endl;

            /* Print decrypted message in hex */
            std::stringstream hex_ss;
            for (unsigned char c : decrypted_message_vec) {
                hex_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            }
            std::cout << "Decrypted message in hex: " << hex_ss.str() << std::endl;

            decrypted_shares.push_back(decrypted_message_vec);
        }

        /* 2. Convert std::vector<unsigned char> to sss_Share and create a std::vector<sss_Share> */
        std::vector<sss_Share> shares(decrypted_shares.size());
        for (size_t i = 0; i < decrypted_shares.size(); ++i) {
            if (decrypted_shares[i].size() > sss_SHARE_LEN) {
                throw std::runtime_error("Decrypted share length must be less than or equal to sss_SHARE_LEN.");
            }
            memcpy(&(shares[i]), decrypted_shares[i].data(), decrypted_shares[i].size());
        }

        /* 3. Call the sss_recombine_password_from_shares function */
        std::string restored_password = sss_recombine_password_from_shares(shares);
        std::cout << "Restored password is: " << restored_password << std::endl;
    }

    return 0;
}
