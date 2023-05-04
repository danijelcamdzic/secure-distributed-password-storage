/**
 * @file ssl_functions.c
 * @brief Contains necessary defines, variables definitions
 * and function definitions for using the ssl sockets in esp32
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include "ssl_functions.h"
#include "MQTT311Client/MQTT311Client.h"

/* ------------------------- VARIABLE DEFINITIONS ------------------------------------ */

/* SSL certificate */
const unsigned char *certificate = (const unsigned char *)"-----BEGIN CERTIFICATE-----\n"
                                                        "MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n"
                                                        "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
                                                        "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n"
                                                        "WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
                                                        "RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
                                                        "AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n"
                                                        "R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n"
                                                        "sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n"
                                                        "NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n"
                                                        "Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n"
                                                        "/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n"
                                                        "AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n"
                                                        "Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n"
                                                        "FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n"
                                                        "AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n"
                                                        "Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n"
                                                        "gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n"
                                                        "PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n"
                                                        "ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n"
                                                        "CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n"
                                                        "lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n"
                                                        "avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n"
                                                        "yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n"
                                                        "yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n"
                                                        "hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n"
                                                        "HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n"
                                                        "MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n"
                                                        "nLRbwHOoq7hHwg==\n"
                                                        "-----END CERTIFICATE-----\n";

/* mbedtls structures */
mbedtls_net_context server_fd;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context ssl;
mbedtls_ssl_config conf;
mbedtls_x509_crt cacert;

/* Connection flag */
static uint8_t connected = 0;

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Connect to a SSL socket using the provided broker address and port.
 *
 * This function resolves the provided broker address to an IP address,
 * creates a SSL socket, and establishes a connection to the specified port.
 *
 * @param brokerAddress The address of the broker to connect to.
 * @param port The port number to establish the connection.
 * @param certificate The PEM format certificate for SSL connection.
 */
void ssl_connect_socket(const char* brokerAddress, uint16_t port)
{
    /* Connect to a SSL socket */
    char* TAG = "ssl_connect_socket";       /**< Declare and initialize TAG for logging purposes */

    int ret;

    mbedtls_net_init(&server_fd);           /**< Initialize mbedtls networking context */
    mbedtls_ssl_init(&ssl);                 /**< Initialize mbedtls SSL context */
    mbedtls_ssl_config_init(&conf);         /**< Initialize mbedtls SSL configuration context */
    mbedtls_x509_crt_init(&cacert);         /**< Initialize mbedtls X.509 certificate context */
    mbedtls_ctr_drbg_init(&ctr_drbg);       /**< Initialize mbedtls counter-mode deterministic random bit generator context */

    mbedtls_entropy_init(&entropy);         /**< Initialize mbedtls entropy context */

    /* Seed the random number generator with entropy */
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned -0x%x", -ret);    /**< Log error if seeding RNG fails */
        return;
    }

    /* Parse the provided X.509 certificate */
    if ((ret = mbedtls_x509_crt_parse(&cacert, certificate, strlen((const char *)certificate) + 1)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);   /**< Log error if certificate parsing fails */
        return;
    }

    /* Set default SSL configuration */
    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned -0x%x", -ret);  /**< Log error if SSL config setup fails */
        return;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);      /**< Set SSL authentication mode */
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);                    /**< Set SSL CA chain */
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);    /**< Set SSL random number generator function */

    /* Set up SSL context with the provided configuration */
    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);        /**< Log error if SSL context setup fails */
        return;
    }

    /* Set the hostname for the SSL context */
    if ((ret = mbedtls_ssl_set_hostname(&ssl, brokerAddress)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret); /**< Log error if hostname setting fails */
        return;
    }

    mbedtls_net_init(&server_fd); /**< Re-initialize mbedtls networking context */

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port); /**< Convert the port number to a string */

    /* Establish a network connection to the provided broker address and port */
    if ((ret = mbedtls_net_connect(&server_fd, brokerAddress, port_str, MBEDTLS_NET_PROTO_TCP)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_net_connect returned -0x%x", -ret); /**< Log error if network connection fails */
        return;
    }

    /* Set up the SSL context with the network connection */
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL); /**< Associate the SSL context with the network context using send and receive functions */

    /* Perform the SSL handshake */
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret); /**< Log error if SSL handshake fails */
            return;
        }
    }

    /* Log SSL connection success */
    ESP_LOGI(TAG, "SSL successfully connected!");

    connected = 1; /**< Set the connected flag to indicate a successful connection */
}

/**
 * @brief Send data over an established SSL connection.
 *
 * This function sends the provided data of a specified size over the
 * previously established SSL connection.
 *
 * @param data Pointer to the data to be sent.
 * @param size The size of the data to be sent.
 */
void ssl_send_data(const char* data, uint16_t size)
{
    if (!connected) return;             /**< Check if the SSL connection has been established; return if not connected */

    /* Send data over SSL connection */
    char* TAG = "ssl_send_data";        /**< Declare and initialize TAG for logging purposes */

    int ret;

    /* Attempt to send data over the SSL connection */
    while ((ret = mbedtls_ssl_write(&ssl, (const unsigned char *)data, size)) <= 0)
    {
        /* Check if the mbedtls_ssl_write function is waiting for read or write operation */
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret); /**< Log error if data sending fails */
            return;
        }
    }

    ESP_LOGI(TAG, "Data sent successfully!"); /**< Log data sending success */
}

/**
 * @brief Set the mbedtls network context to non-blocking mode.
 *
 * This function sets the file descriptor of the given mbedtls network context to non-blocking mode.
 * In non-blocking mode, the file descriptor will not wait for an operation to complete before returning.
 *
 * @param[in,out] ctx A pointer to the mbedtls_net_context structure to be set to non-blocking mode.
 */
static void mbedtls_net_set_nonblocking(mbedtls_net_context *ctx)
{
    /* Attempt to set the mbedtls network context's file descriptor to non-blocking mode */
    if (fcntl(ctx->fd, F_SETFL, fcntl(ctx->fd, F_GETFL) | O_NONBLOCK) < 0)
    {
        perror("mbedtls_net_set_nonblock"); /**< Print error message if setting to non-blocking mode fails */
    }
}

/**
 * @brief Receive data from an established SSL connection.
 *
 * This function reads data from the SSL connection in a non-blocking mode
 * and stores it in the MQTT311_RECEIVE_BUFFER. It also logs the number of
 * bytes received and any errors that may occur during reception.
 */
void ssl_receive_data(void)
{
    if (!connected) return;                                             /**< Check if the SSL connection has been established; return if not connected */

    char* TAG = "ssl_receive_data";                                     /**< Declare and initialize TAG for logging purposes */

    /* Set SSL socket to non-blocking mode */
    mbedtls_net_set_nonblocking(&server_fd);

    /* Receive everything in a loop until there is no data to receive */
    while(1) {
        char temp_buffer[15];                                           /**< Define a temporary buffer of length 15 */

        /* Receive data from the SSL connection */
        int ret = mbedtls_ssl_read(&ssl, (unsigned char *)temp_buffer, sizeof(temp_buffer) - 1);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGI(TAG, "No data to read...");                         /**< Log if no data is available to read */
            break;
        } else if (ret < 0) {
            ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);      /**< Log if an error occurred during receiving */
            break;
        } else if (ret == 0) {
            ESP_LOGI(TAG, "Connection closed by peer");                  /**< Log if connection closed by the peer */
            break;
        } else {
            /* Concatenate the received data to the existing data in the MQTT311_RECEIVE_BUFFER */
            memcpy((void *)(MQTT311_RECEIVE_BUFFER + MQTT311_RECEIVED_BYTES), (const void *) temp_buffer, ret);

            MQTT311_RECEIVED_BYTES += ret;                              /**< Increment the number of bytes received */

            ESP_LOGI(TAG, "Received %d bytes from the SSL connection", ret);
        }
        /* Wait a short period */
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
