# About
This project implements the **Secure Distributed Password Storage** system that is proposed in my master thesis. Here is an abstract taken from my thesis:

"_This thesis aims to present a new system for storing and recovering passwords and other types of sensitive information. It attempts to solve problems that centralized solutions suffer from and bring the power of data possession back into the hands of users. It demonstrates a low-cost, space-effective, distributed solution with the top priority being the security and privacy of the user's data._

_The system presented is based on a master/slave model in the form of software/hardware node entities which employ the publish/subscribe model of the MQTT communication protocol. The underlying security measures utilize the RSA Encryption method and Shamir's Secret Sharing Scheme._

_The basis of the system is the exchange of RSA encrypted MQTT messages, whose contents are the password pieces split up by the Shamir's Secret Sharing Scheme, between the software node and a cluster of hardware nodes that comprise the system._

_Through the analysis and practical demonstration, this thesis aims to demonstrate the robustness and security of the system, showcasing its advantages in terms of resistance to attacks, protection of confidential data, efficiency in recovering secret information, and the superiority of distributed over centralized data storage._"

# Detailed Documentation
For more in-depth information, see the following documents:

- [System Architecture](doc/system_architecture.md)
- [System Design](doc/system_design.md)
- [Results](doc/results.md)

To see this system in action, check out the demonstration video by clicking [here](demonstration.mp4).

# Usage

This section will guide you through the process of using the SDPSR system. We'll start with the software node.

## Software Node

To use the software node, follow these steps:

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
const std::string SERVER_ADDRESS("tcp://test.mosquitto.org:1883");
const std::string CLIENT_ID("access_node");
const std::string SERVER_CERTICIATE_PATH("../mosquitto.org.pem");    /**< Can be empty if only TCP connection will be used*/

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


## Hardware Nodes

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

> The public and the private keys are stored as "const" variables which means that they are stored in the FLASH by default on the ESP32. Even though it is true that these keys protect only one password share that is being delivered and stored on a particular ESP32 device, a skilled attacker can find these keys by using some flash dump software and obtain that one password share which may or may not mean much. This presumes that the ESP32 has been successfully stolen from the user. 

For users who want additional protection against unauthorized acquiring of the RSA keys by an attacker, the entire flash contents of the ESP32 can be encrypted. Follow the [official](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/security/flash-encryption.html) documentation on how to do this. Bear in mind, however, that this process is irreversible and creates additional keys to remember and save for each ESP32 that flash encryption is applied to.

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

# Installation

This section will guide you through the process of setting up the necessary tools to use the Secure Distributed Password Storage and Recovery system. Start by cloning the repository recursively:

```bash
git clone --recursive https://github.com/danijelcamdzic/secure-distributed-password-storage.git
```

Now, let's set up the environment for the software node and the hardware nodes.

## Software node

To set up the software node, we'll need to install the Paho MQTT C++ library, OpenSSL, and CMake. Follow the instructions below:

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

## Hardware Nodes

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



