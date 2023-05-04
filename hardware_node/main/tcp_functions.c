/**
 * @file tcp_functions.c
 * @brief Contains necessary defines, variables definitions
 * and function definitions for using the tcp sockets in esp32
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include "tcp_functions.h"
#include "MQTT311Client/MQTT311Client.h"

/* ------------------------------- DEFINES ------------------------------------------ */

/* Defines for configuring the TCP Socket for IPv4 */
#ifndef CONFIG_EXAMPLE_IPV4
#define CONFIG_EXAMPLE_IPV4
#endif

/* ------------------------- VARIABLE DEFINITIONS ------------------------------------ */

/* TCP Socket */
static int sock_tcp;

/* Connection flag */
static uint8_t connected = 0;

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Connect to a TCP socket using the provided broker address and port.
 *
 * This function resolves the provided broker address to an IP address,
 * creates a TCP socket, and establishes a connection to the specified port.
 *
 * @param brokerAddress The address of the broker to connect to.
 * @param port The port number to establish the connection.
 */
void tcp_connect_socket(const char* brokerAddress, uint16_t port) 
{
    /* Connect to a TCP socket */
    char* TAG = "tcp_connect_socket";                                   /**< Declare and initialize TAG for logging purposes */

    int addr_family = 0;
    int ip_protocol = 0;
    struct sockaddr_in dest_addr;

    /* Configure for IPv4 */
#if defined(CONFIG_EXAMPLE_IPV4)
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));                                   /**< Initialize hints structure with zeroes */
    hints.ai_family = AF_INET;                                          /**< Specify address family as IPv4 */
    hints.ai_socktype = SOCK_STREAM;                                    /**<  Specify socket type as stream */
    hints.ai_protocol = IPPROTO_IP;                                     /**<  Specify IP protocol */

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);                   /**< Convert port to string */

    /* Resolve DNS name */
    int result = getaddrinfo(brokerAddress, port_str, &hints, &res);    /**< Get address info of the broker */
    if (result != 0) {
        ESP_LOGI(TAG, "Could not resolve DNS name...");                 /**< Log if DNS name resolution failed */
        return;
    }

    memcpy(&dest_addr, res->ai_addr, res->ai_addrlen);                  /**< Copy resolved address to destination address structure */
    addr_family = AF_INET;                                              /**< Set address family to IPv4 */
    ip_protocol = IPPROTO_IP;                                           /**< Set IP protocol */
    freeaddrinfo(res);                                                  /**< Free the memory allocated for address info */
#endif

    /* Create the socket */
    sock_tcp = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock_tcp < 0) {
        ESP_LOGI(TAG, "Unable to create socket!");                      /**< Log if socket creation failed */
        return;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", brokerAddress, port); /**< Log socket creation success */

    /* Connect to the socket */
    int err = connect(sock_tcp, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGI(TAG, "Socket unable to connect!");                     /**< Log if socket connection failed */
        close(sock_tcp);                                                /**< Close the socket */
        return;
    }
    ESP_LOGI(TAG, "Socket successfully connected!");                    /**< Log socket connection success */

    /* Set socket to non-blocking mode */
    if (fcntl(sock_tcp, F_SETFL, fcntl(sock_tcp, F_GETFL) | O_NONBLOCK) < 0) {
        ESP_LOGI(TAG, "Cannot put socket in non-blocking mode!");       /**< Print error message if setting to non-blocking mode fails */
    } else {
        ESP_LOGI(TAG, "Socket set to non-blocking mode...");
        connected = 1;  /**< Set the connected flag to indicate a successful connection */
    }
}

/**
 * @brief Send data over an established TCP connection.
 *
 * This function sends the provided data of a specified size over the
 * previously established TCP connection.
 *
 * @param data Pointer to the data to be sent.
 * @param size The size of the data to be sent.
 */
void tcp_send_data(const char* data, uint16_t size)
{
    if (!connected) return;                                 /**< Check if the TCP connection has been established; return if not connected */

    /* Send data over TCP connection */
    char* TAG = "tcp_send_data";                            /**< Declare and initialize TAG for logging purposes */

    /* Send data through the socket */
    int err = send(sock_tcp, data, size, 0);           

    /* Check if data was sent successfully */
    if (err < 0) {
        ESP_LOGI(TAG, "Failed to send data!");              /**< Log if data sending failed */
    }
}

/**
 * @brief Receive data from an established TCP connection.
 *
 * This function reads data from the TCP connection in a non-blocking mode
 * and stores it in the MQTT311_RECEIVE_BUFFER. It also logs the number of
 * bytes received and any errors that may occur during reception.
 */
void tcp_receive_data(void)
{
    if (!connected) return;                                             /**< Check if the TCP connection has been established; return if not connected */

    char* TAG = "tcp_receive_data";                                     /**< Declare and initialize TAG for logging purposes */                                                

    /* Receive everything in a loop until there is no data to receive */
    while(1) {
        char temp_buffer[15];                                           /**< Define a temporary buffer of length 15 */

        /* Receive data from the socket */
        int len = recv(sock_tcp, temp_buffer, sizeof(temp_buffer) - 1, 0);

        if (len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            ESP_LOGI(TAG, "No data to read...");                         /**< Log if no data is available to read */
            break;
        } else if (len < 0) {
            ESP_LOGI(TAG, "Error during reception!");                   /**< Log if an error occurred during receiving */
            break;
        } else {
            /* Concatenate the received data to the existing data in the MQTT311_RECEIVE_BUFFER */
            memcpy((void *)(MQTT311_RECEIVE_BUFFER + MQTT311_RECEIVED_BYTES), (const void *) temp_buffer, len);

            MQTT311_RECEIVED_BYTES += len;                              /**< Increment the number of bytes received */

            ESP_LOGI(TAG, "Received %d bytes from the socket", len);
        }
        /* Wait a short period */
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
