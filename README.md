# About
Sections below provide an overview of the system.

## Title
The title of the project is: "Secure Distributed Password Storage and Recovery on MQTT-connected Hardware IoT Nodes using RSA Cryptography and Shamir's Secret Sharing Scheme"

## Overview
This master's thesis project involves the analysis and development of a secure, distributed system for storing and accessing secret passwords across multiple independent hardware IoT devices. The system utilizes the MQTT 3.1.1 protocol, RSA cryptography, and Shamir's Secret Sharing scheme.

## Software Access Node
The system demonstrates the use of a software access node implemented in C++, along with multiple hardware devices (nodes) with internet access, specifically ESP32 devices. Each node in the system has its own public and private RSA (2048-bit) key. The cross-platform software access node can run on any platform that supports C++ compilation and execution. It implements RSA encryption, RSA decryption, as well as the splitting and reconstruction of passwords using Shamir's Secret Sharing formula.

## Hardware Nodes
Hardware devices in the system act as storage nodes for password fragments. The system relies on IoT connections between devices by implementing the MQTT 3.1.1 protocol, with a custom [MQTT 3.1.1 client library](https://github.com/danijelcamdzic/mqtt311-client-library) built from scratch, using FreeRTOS concurrency mechanisms, and fulfilling all requirements and functionalities specified by the MQTT 3.1.1 standard. This eliminates the need for third-party implementations.

## Additional Security Features
Hardware devices in the system also implement RSA encryption and RSA decryption using the Mbed-TLS library. The system is designed so that the software access node has public RSA (2048-bit) keys for all hardware nodes, while hardware nodes only have the public key for the access node. Passwords created via the access node are split into N parts (where N is the number of hardware nodes), with M (M<=N) parts required for password reconstruction. Each password part is encrypted with the hardware node's RSA (2048-bit) public key before being sent.

The system is based on the fact that each hardware node listens to a specific topic via the MQTT 3.1.1 protocol and receives its encrypted password part, which is decrypted using its private RSA (2048-bit) key and immediately encrypted with the access node's public RSA (2048-bit) key before being stored in persistent memory. The foundation of the system is that when retrieving a password, the hardware nodes send their encrypted password parts to the control node, which is the only entity capable of decrypting them with its private RSA (2048-bit) key and reconstructing the original password using Shamir's formula.

## Flexibility and Security
The system provides enhanced flexibility by allowing users to choose the number of required hardware nodes for password restoration during the splitting process. This offers additional security and protection against unauthorized access, data loss, or system compromise due to individual failures or hacking attempts. Through the implementation and practical application of the system, the goal of the project is to demonstrate the robustness and security of this system, showcasing its advantages in terms of resistance to attacks, protection of confidential data, efficiency in recovering secret information, and the superiority of distributed storage over centralized data storage.
