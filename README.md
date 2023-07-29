# About
This project implements the **Secure Distributed Password Storage** system that is proposed in my master thesis. Here a an abstract takes from my thesis:

" _This thesis aims to present a new system for storing and recovering passwords and other types of sensitive information. It attempts to solve problems that centralized solutions suffer from and bring the power of data possession back into the hands of users. It demonstrates a low-cost, space-effective, distributed solution with the top priority being the security and privacy of the user's data._

_The system presented is based on a master/slave model in the form of software/hardware node entities which employ the publish/subscribe model of the MQTT communication protocol. The underlying security measures utilize the RSA Encryption method and Shamir's Secret Sharing Scheme._

_The basis of the system is the exchange of RSA encrypted MQTT messages, whose contents are the password pieces split up by the Shamir's Secret Sharing Scheme, between the software node and a cluster of hardware nodes that comprise the system._

_Through the analysis and practical demonstration, this thesis aims to demonstrate the robustness and security of the system, showcasing its advantages in terms of resistance to attacks, protection of confidential data, efficiency in recovering secret information, and the superiority of distributed over centralized data storage._"

# System Architecture

This section presents the architectural model that the software node and hardware nodes use. It includes detailed descriptions of the components that comprise the node's interior structure as well as the libraries that are utilized by those components and the main application as well. As this entire chapter is dedicated to the demonstration of the system based on a practical real-world use case scenario, all the details regarding the components and the choices that have been made are laid out as they are in this repository.

## Software Node

The software node is an application written in C++ programming language. It was developed and intended to be run on a device that supports the C++ build system - a laptop as it was used in this implementation, or some other device with similar capabilities. It consists of the main application, components that facilitate the main code, and the libraries that are employed by the components and the main application.

The software node does not feature an operating system as it is intended to be used for specific purposes for a limited duration of time after which it is exited. It consists of three layers:

* **Application Layer:** This layer features the main application which includes the main function and other helper functions which facilitate the main code.
	
* **Component Layer:** This layer features the components which are used by the main code to run the system.
	
* **Library Layer:** This layer features the libraries used by the components in the component layer to facilitate the execution of specific software tasks.

The application layer features the main application which contains two functions - a function to create a password and a function to run the main code. The main code is where all the action is happening based on the arguments with which the software node program is called. It either runs the password storage process or the password recovery process. The function which creates a password allows the user to choose a password while providing a secure way to do so by utilizing the termios [^1] library.

The component layer features different components whose purpose has been the thorough discussion of the preceding chapters. These components include:

1. **MQTT Component:** Provides a set of MQTT related functions.
	
2. **RSA Component:** Provides a set of functions connected to the RSA Encryption method.
	
3. **SSS Component:** Provides a set of functions connected to the Shamir's Secret Sharing Scheme.

The library layer features a set of software libraries that are used by the components from the component layer and which provide a set of APIs to the methods abstracted to the main application by the component layer. These libraries are:

1. **Paho MQTT Library:** This library provides MQTT features to the device. It gives it an ability to be an MQTT client and exchange messages over the MQTT protocol.
	
2. **OpenSSL Library:** This library includes many cryptograhic methods among which are the RSA key generation, the RSA encryption and the RSA decryption.
	
3. **SSS Library:** This library implements the Shamir's Secret Sharing Scheme and provides an API for the functions which split up the password and reassemble the password shares.

The figure below provides a graphical representation of the architectural model that the software node uses. It clearly depicts the architectural layers described, the components utilized by the main application, and the libraries used by the components.

![The Architecture Model of the Software Node](assets/software_node_architecture.png)

## Hardware Nodes

Hardware nodes represent a cluster of small resource-constrained embedded devices with internet access. In contrast to the software node, the hardware nodes are meant to be network-connected all the time (although occasional disconnections are not a problem if they are not occurring due to the fault of the device). This is because they are supposed to listen and wait for commands telling them to send their password shares back in order for the user to recover the password.

In this practical implementation, a cluster of ESP32 devices was chosen to play the role of hardware nodes. The choice was made due to a number of reasons which include cost-effectiveness (as ESP32 devices are really cheap), low-power consumption, ease of programming (supports multiple programming languages) and built in WiFi connectivity support (which reduces the hassle to have devices be network connected all the time) among many others.

The ESP32 devices, that are our hardware nodes, feature an operating system, a FreeRTOS library, mainly due to the incorporation of the MQTT 3.1.1 client library which utilizes FreeRTOS from the get-go.

The hardware nodes' architectural model is comprised of the same three layers as in the software node's case. It features an application layer, a component layer, and a library layer with the same principles in mind.

The application layer features the main application which in the case of the hardware nodes contains the main code function and a WiFi initialization function. ESP32 provides a convenient way to connect to the WiFi with the utilization of the esp_wifi library which comes with the ESP32 software development tools. The main code serves to start the WiFi connection process, establish a connection between the device and the chosen MQTT broker, subscribe to relevant topics, and initialize the non-volatile memory of the device. For the MQTT-related procedures, it uses the MQTT 3.1.1 client library which was the topic of discussion in chapter 4 and for the non-volatile memory procedures, it uses the NVS component from the component layer.

The component layer features a number of components with various purposes. These components are:

1. **MQTT Component:** Provides a set of MQTT related functions.
2. **RSA Component:** Provides a set of functions related to the RSA Encryption method.
3. **NVS Component:** Provides a set of functions related to the handling of the non-volatile memory of the ESP32 device.
4. **SSL Component:** Provides a set of functions for opening an SSL socket as well as sending and reading data through it.
5. **TCP Component:** Provides a set of functions for opening an TCP socket as well as sending and reading data through it.

The library layer features a number of software libraries for the ESP32 devices which, for the purposes of this real-world use case, are:

1. **MQTT 3.1.1 Client Library:** This library provides MQTT 3.1.1 features to small embedded devices with low-power consumption and turns them into MQTT 3.1.1 client devices.
2. **NVS Library:** This library includes API for working with the non-volatile memory of the ESP32 device.
3. **MbedTLS Library:** This library includes cryptographic methods used by the ESP32 device, including SSL socket functionality (for the encryption layer on top of TCP) as well as RSA Encryption method functionality.
4. **LWIP Library:** This library includes API for working with TCP sockets.

The figure below provides a graphical representation of the architectural model that the hardware node uses. It clearly depicts the architectural layers described, components utilized by the main application and library used by the components.

![The Architecture Model of the Hardware Node](assets/hardware_node_architecture.png)

# The Design

This section showcases the design model that the software node and hardware nodes follow. Representing the design model of the system means describing a set of procedures it performs in order to successfully complete an operation. Since the two main operations present in both the software node and the hardware nodes are the password storage operation and password recovery operation, this section will focus on encapsulating the design model of the main application for both types of nodes. This means that the design flow will be seen from the perspective of the main application and the surrounding components will come into play once their functionality is needed upon in the execution of the operation.

To simplify the explanations and the corresponding diagrams, there will be no distinction made between the component layer and the library layer and the outside factors (not present in the main application) will be denoted by their functionality type. This would mean, for example, that both the MQTT 3.1.1 client library from the library layer and the MQTT component from the component layer represent the same entity providing MQTT functionality.

## Software Node

As was stated in several places throughout this thesis, the software node performs two processes - the password storage process and the password recovery process. Since these two processes have been explained thoroughly through the analysis of their key components in various sections of this thesis, the following two figures serve the purpose of providing a clear representation of the design model through the practical real-world use of this system.

### Password Storage Process

The figure below represents the sequence diagram of the password storage process as seen from the perspective of the software node.

![Password Storage Process (Software Node)](assets/software_node_design_storage.png)

### Password Recovery Process

The figure below represents the sequence diagram of the password recovery process as seen from the perspective of the software node.

![Password Recovery Process (Software Node)](assets/software_node_design_recovery.png)

## Hardware Nodes

Exactly the same as in the software node case, the hardware nodes perform two of the same processes as well. Since the hardware nodes operate under the conditions of the operating system FreeRTOS, it is more difficult to represent their design flow but the best attempt was made with the use of the following two sequence diagrams.

### Password Storage Process

The figure below represents the sequence diagram of the password storage process as seen from the perspective of the hardware node.

![Password Storage Process (Hardware Node)](assets/hardware_node_design_storage.png)

### Password Recovery Operation

The figure below represents the sequence diagram of the password recovery process as seen from the perspective of the hardware node.

![Password Recovery Process (Hardware Node)](assets/hardware_node_design_recovery.png)

# Results

In order to observe how the system works and behaves under different scenarios, a real-world case described in the previous chapter was utilized. The software node is run on a laptop running GNU/Linux operating system and the two hardware nodes which comprise the system are the ESP32 devices.

In the figures below, terminal window 1 represents the software node and terminal windows 2 and 3 represent the ESP32 devices.

The figure below represents a screenshot of a laptop screen where the software node application was run. It demonstrates the password storage process.

![Password Storage Process](assets/app_save_password.png)

From the debug information present in the terminal windows it can be followed how the nodes completed their tasks. The software node sent the password pieces, which got stored in the non-volatile memory of the ESP32 devices which consequently sent confirmation messages back to the software node, effectively ending the process.

The figure below also represents a screenshot of the laptop screen but with a password recovery process in display.

![Password Recovery Process](assets/app_get_password.png)

From the debug information, it can be observed that when the software node sends a command to recover the password, the ESP32 devices receive it, respond by sending the password pieces back and the software node reconstructs the original password.

The figure below represents a screenshot of a laptop screen depicting a scenario in which one of the hardware nodes goes offline.

![Password Recovery Process when 1 Hardware Node is Offline](assets/app_get_password2.png)

From the debug information, it is seen that ESP32 device on the terminal window 3 went offline and is unable to respond to the password recovery request. The password piece is only sent by the ESP32 device from the terminal window 2 but the original password is still correctly reconstructed as the threshold value in this system was set to 1.

# Installation

This section will guide you through the process of setting up the necessary tools to use the Secure Distributed Password Storage and Recovery system. Start by cloning the repository recursively:

```bash
git clone --recursive https://github.com/danijelcamdzic/secure-distributed-password-storage.git
```

Now, let's set up the environment for the software access node and the hardware nodes.

## Software Access Node

To set up the software access node, we'll need to install the Paho MQTT C++ library, OpenSSL, and CMake. Follow the instructions below:

### Install Paho MQTT C++ library

1. Install the required dependencies:

```bash
sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev
```

2. Clone the Paho MQTT C++ library repository:

```bash
git clone https://github.com/eclipse/paho.mqtt.cpp
```

3. Build and install the library

```bash
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_ENABLE_TESTING=FALSE
sudo cmake --build build/ --target install
sudo ldconfig
```

### Install OpenSSL

1. Update the package index: 

```bash
sudo apt-get update
```

2. Install OpenSSL:

```
sudo apt-get install libssl-dev
```

### Build the project using CMake

1. Navigate to the `software_node` directory:

```bash
cd secure-distributed-password-storage/software_node
```

2. Create a build directory and run CMake:

```bash
mkdir build
cd build
cmake ..
```

3. Compile the project

```bash
make
```

## Hardware Node

To set up the hardware nodes, follow the steps to set up the ESP32 build environment as described in the official [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html) of the esp32.

### Build the project using `idf.py`:

1. Navigate to the `hardware_node` directory:

```bash
cd secure-distributed-password-storage/hardware_node
```

2. Export necessary tools:

```bash
 . $HOME/esp/esp-idf/export.sh
```

3. Edit the `menuconfig` to edit your Wi-Fi credentials:

```bash
idf.py menuconfig
```

4. Build the project

```bash
idf.py build
```


4. Flash the project

```bash
idf.py flash
```

# Usage

This section will guide you through the process of using the Secure Distributed Password Storage and Recovery system. We'll start with the software access node.

## Software Node

To use the software access node, follow these steps:

1. Add the public RSA (2048-bit) keys of the hardware nodes to the software_node folder in .pem format. Also, add your private and public RSA 2048-bit keys to the same folder in .pem format. To generate an RSA 2048-bit key in PEM format, use the following OpenSSL command:

```bash
openssl genpkey -algorithm RSA -out private_key.pem -pkeyopt rsa_keygen_bits:2048
openssl rsa -pubout -in private_key.pem -out public_key.pem
```

2. Modify the following values in `shamir_secret_sharing_functions.cpp` according to your requirements:

```cpp
#define SHAMIR_NUM_SHARES   1
#define SHAMIR_THRESHOLD    1
```

`SHAMIR_NUM_SHARES` should be equal to the number of hardware nodes that the shares are being sent to, and `SHAMIR_THRESHOLD` must be less than or equal to `SHAMIR_NUM_SHARES` and represents the reconstruction threshold.

3. In `rsa_functions.cpp`, update the file paths and list all the public keys of the hardware nodes:


```cpp
const std::string RSA_PUBLIC_KEY("../rsa_public_key.pem");
const std::string RSA_PRIVATE_KEY("../rsa_private_key.pem");

const std::string RSA_PUBLIC_KEY_HW_NODE_1("../rsa_public_key_hw_node_1.pem");

const std::vector<std::string> public_keys_hw_nodes = {RSA_PUBLIC_KEY_HW_NODE_1};
```

4. Configure the MQTT settings in `mqtt_functions.cpp`:

```cpp
/* Variable for general broker connection */
const std::string SERVER_ADDRESS("tcp://mqtt.eclipseprojects.io:1883");
const std::string CLIENT_ID("access_node");
const std::string SERVER_CERTICIATE_PATH("../mqtt_ecplipseprojects_io_certificate.pem");    /**< Can be empty if only TCP connection will be used*/

/* Variable for commanding the hardware nodes */
const std::string RETRIEVE_PASSWORD_COMMAND("GetPassEND_MESSAGE");          /**< Used in the restore password command */
const std::string END_MESSAGE_FLAG("END_MESSAGE");                          /**< Sent at the end of every encrypted message to hardware nodes */

/* Variable for communication with hardware nodes */
const std::string TOPIC_SUB_HW_1("/topic/sub/hw_node_1");                   /**< On this topic the hardware node 1 sends messages */
const std::string TOPIC_PUB_HW_1("/topic/pub/hw_node_1");                   /**< On this topic the hardware node 1 listens for messages */
/* ... Add more topics for subscription and publishing (one for each hardware node) */
const std::string TOPIC_PUB_ALL("/topic/pub/all");                          /**< On this topic all hardware nodes are listening */

/* Vectors for containing the publish and subscribe topics */
const std::vector<std::string> sub_topics = {TOPIC_SUB_HW_1};               /**< Add subscription topics for each hardware node in the vector */
const std::vector<std::string> pub_topics = {TOPIC_PUB_HW_1};  
```

5. To use the `main` app, execute it with either `save_password` or `get_password`:

```
./access_node save_password
```

or 

```bash
./access_node get_password
```

You will be prompted for your MQTT credentials and the password to save, or you will receive a password, depending on the command you used.


## Hardware Node

1. In the `RSA_Encrypt.c` file, add the software node's (master) public key:

```cpp
/* Master's Public RSA key */
const unsigned char *masterkey = (const unsigned char *)"-----BEGIN PUBLIC KEY-----\n"
                                                        ...
                                                        "-----END PUBLIC KEY-----\n";
```

2. In the `RSA_Decrypt.c` file, add the hardware node's private key:

```cpp
/* Private RSA key (hardware node) */
const unsigned char *private_key = (const unsigned char *)"-----BEGIN PRIVATE KEY-----\n"
                                                            ...
                                                            "-----END PRIVATE KEY-----\n";
```

3. Edit MQTT configuration:

```cpp
/* MQTT connection specific defines */
#define BROKER_ADDRESS              "mqtt.eclipseprojects.io"
#define BROKER_PORT_TCP             1883
#define BROKER_PORT_SSL             8883
#define KEEP_ALIVE                  120
#define CLIENT_ID                   "hardware_node_1"
#define USERNAME                    ""
#define PASSWORD                    ""

/* SSL certificate */
extern const unsigned char *certificate;

/* Topics for subscribing and publishing the data */
#define SUB_TOPIC                   "/topic/pub/hw_node_1"          /**< On this topic the encrypted data is received */
#define PUB_TOPIC                   "/topic/sub/hw_node_1"          /**< On this topic the data is sent back */
#define ALL_TOPIC                   "/topic/pub/all"                /**< On this topic the command to send data back is received */

```

Make sure to add the full certificate if you are using `ssl_functions.h` to connect to the MQTT broker.

Also make sure to set the `CLIENT_ID` to a unique value for each hardware node.

After configuring the RSA keys and MQTT settings, the hardware node is ready to be used with the software node.



