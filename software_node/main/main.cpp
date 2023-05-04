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

/* Libraries for secure password/password retrieval from console */
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

#ifdef DEBUG
/**
 * @brief This function prints debugging information from the received vector including the size and
 * its hex form
 *
 * @param data vector of unsigned chars
 */
void debug_print(const std::vector<unsigned char> data)
{
    /* Print the vector length */
    std::cout << "Vector length: " << data.size() << std::endl;

    /* Print the vector in readable hex form */
    std::stringstream hex_ss;
    for (unsigned char c : data) {
        hex_ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    std::cout << "Vector in hex: " << hex_ss.str() << std::endl;
}

/**
 * @brief This function prints debugging information from the received string including the size and
 * its contents
 *
 * @param data string
 */
void debug_print(const std::string data)
{
    /* Print the vector length */
    std::cout << "String length is: " << data.size() << std::endl;

    /* Print the string */
    std::cout << "String: " << data << std::endl;
}
#endif

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

    /* Get username */
    std::string mqtt_username;
    std::cout << "Please enter your MQTT username: ";
    std::getline(std::cin, mqtt_username);

    /* Get password */
    std::string mqtt_password;
    std::cout << "Please enter your MQTT password: ";
    mqtt_password = read_password();

    /* Connect to the MQTT broker (SSL if certificate included in call, TCP otherwise) */
    mqtt_connect(mqtt_username, mqtt_password, SERVER_CERTICIATE_PATH);
    /* Subscribe to all topics from the list */
    for (const std::string& topic : sub_topics) {
        mqtt_subscribe(topic);
    }

    /* Save password taken from the console */
    if (option == "save_password") {
        /* Read the password from the console */
        std::cout << "Please enter the password which you wish to save: ";
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

#ifdef DEBUG
            /* See the length of the original share and its contents */
            std::cout << "Printing the share_data.." << std::endl;
            debug_print(share_data);
#endif

            /* Encrypt share with the correct public key of the hardware node */
            encrypted_shares[i] = rsa_encrypt_message(share_data, public_keys_hw_nodes[i]);

            /* Append the END_MESSAGE_FLAG */
            std::vector<unsigned char> end_message_flag_vec(END_MESSAGE_FLAG.begin(), END_MESSAGE_FLAG.end());
            encrypted_shares[i].insert(encrypted_shares[i].end(), end_message_flag_vec.begin(), end_message_flag_vec.end());

            /* Publish the share to the correct topic of the hardware node */
            mqtt_publish(pub_topics[i], encrypted_shares[i]);
        }

        /* Wait for confirmation of reception from the hardware nodes (wait for SHAMIR_NUM_SHARES confirmations) */
        mqttCallbackFunction.wait_for_messages(SHAMIR_NUM_SHARES, WAIT_PERIOD_MS);

        /** Check if the received messages are OK
        *   This block of code functions properly because the only topics this app is subscribing to are the ones
        *   from which the hardware nodes are sending data. No other topics should be subscribed.
        *   If other topics are subscribed to, this part will not function and the wait_for_messages function should be
        *   edited to wait only for topics that are concerning the hardware nodes communication.
        */
        auto received_messages = mqttCallbackFunction.get_received_messages();
        for (const auto& [topic, message] : received_messages) {
#ifdef DEBUG
            /* See the length of the topic and message and its contents */
            std::cout << "Printing the topic.." << std::endl;
            debug_print(std::string(topic.begin(), topic.end()));
            std::cout << "Printing the message.." << std::endl;
            debug_print(std::string(message.begin(), message.end()));
#endif

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
        std::cout << "The shares have been sent and confirmed by the hardware nodes!" << std::endl;
    } 
    /* Retrieve password from the hardware nodes */
    else {
        /* Send command to retrieve the password from hardware nodes */
        std::vector<unsigned char> retrieve_message_command(RETRIEVE_PASSWORD_COMMAND.begin(), RETRIEVE_PASSWORD_COMMAND.end());
        mqtt_publish(TOPIC_PUB_ALL, retrieve_message_command);

        /* Wait for the minimum number of messages */
        mqttCallbackFunction.wait_for_messages(SHAMIR_THRESHOLD, WAIT_PERIOD_MS);

        /* Fetch the messages */
        auto received_messages = mqttCallbackFunction.get_received_messages();

        /* 1. Decrypt the messages using rsa_decrypt_message */
        std::vector<std::vector<unsigned char>> decrypted_shares;
        for (const auto& [topic, message] : received_messages) {
            /* Transform the received message */
            std::vector<unsigned char> encrypted_message_vec(message.begin(), message.end());

#ifdef DEBUG
            /* See the length of the topic and message and its contents */
            std::cout << "Printing the topic" << std::endl;
            debug_print(std::string(topic.begin(), topic.end()));
            std::cout << "Printing the encrypted message vector.." << std::endl;
            debug_print(encrypted_message_vec);
#endif

            /* Decrypt with the private key of the master device */
            std::vector<unsigned char> decrypted_message_vec = rsa_decrypt_message(encrypted_message_vec, RSA_PRIVATE_KEY);

#ifdef DEBUG
            /* See the length of and the contents of the decrypted message*/
            std::cout << "Printing the decrypted message vector.." << std::endl;
            debug_print(decrypted_message_vec);
#endif

            /* Push the decrypted share into the vector */
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
