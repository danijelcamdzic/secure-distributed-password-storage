/* Secure Destributed Password Storage

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <netdb.h>
#include <fcntl.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "MQTT311/MQTT311.h"

#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/platform.h"
#include "mbedtls/base64.h"

/* This uses WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/* Defines for configuring the TCP Socket for IPv4 */
#ifndef CONFIG_EXAMPLE_IPV4
#define CONFIG_EXAMPLE_IPV4
#endif

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* TCP Socket */
int sock;
unsigned char buf[MBEDTLS_MPI_MAX_SIZE] = {0};
    const unsigned char *private_key = (const unsigned char *)"-----BEGIN PRIVATE KEY-----\n"
                                                            "MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQC240GJf9WuE/vW\n"
                                                            "z7JMinulBsOQZ8JBgmxrZPDRDg/8JpeINU5r58p/0rEfrQv4xTbiMa4/zeAoDv0M\n"
                                                            "X4eLT3yAuoKjnPxPPiWI1XgjvzNsJQ330RlIWSvSVIXb/yRFOh4GMhDxE7FlDciQ\n"
                                                            "1tCov4Q4aDAPf2mt84UEeIg2G1g/TUe7Pyp4a01RepzaWjvS9DGbp9EX7P7orYJd\n"
                                                            "d/XHpzw8wu+sTlp1EzfPf5kPAu5DeJgZYh1x1zTzVezpQzMaA1/WyzybpknPESVi\n"
                                                            "rJewIjp5TflWvn7tAiY5gNecmL7UiIQeNoGITKkmHd0mTubWgYvNHcUDaFW6OhNc\n"
                                                            "xayqOZanAgMBAAECggEAGLcm2NwhFmtg8gC+NGsqlny5hRRgmNXKrSDGYponPYjs\n"
                                                            "RYi0AQ/MD/DQkFqkbt7IB1LLckKc87eYKZ88rsOegbmy2fA8hhZR9ZH+WxEFyVb1\n"
                                                            "8khHGxM3QYS7/6UVU72s57gbDgP+nzd4+V9Dx5T022KedsRd6xd+dxeK4N+v0gNM\n"
                                                            "aqaSyzCxWwHkUN1g8nZgIqe/UM0N/cpGsiF4tUZSb+m8oUtXVz4Phz6ismaQv0yH\n"
                                                            "B/nRJkpjdI7VAeSnQOoBQy1EvO37cs5gk2+T+FaHM3qQlXGYzPoyG1+mLs/Zarpd\n"
                                                            "vgNNWEfRhVkEpysI7dqbbv56bnEKn5cVH3m6j5YeAQKBgQDf17VAdL+YTnkI7GQu\n"
                                                            "uK1jz/+ESCcdlSFVIFryBuCEPra3E57J6vkzwg41Us6tW3qOsikqgLiGZAUIDReH\n"
                                                            "PgXQqvoVpygCSpux8LTMVvrAwOi/qO/hIPVGIg5wx6L5Ot58rqnAI1j879O0apRA\n"
                                                            "cCJT1eWFRAefUnMxNxnfZi3EJwKBgQDRKVgUPc2u7l7h/TZhH7EUQT7WRFNbFmCX\n"
                                                            "8YNLR8ojR7YbkMRH1jJBGafAZKnJJeOH+OqkbvIx8+9S8m/kWg5WlHm6MLXD/0VG\n"
                                                            "YN8C3fx1jsxuzkCXK8EOQV2pLxWPfjrsvGz1oLF8ixSqfbpM4o9u4IW6/X/v/BYP\n"
                                                            "1tBCY3SpgQKBgQDRTYHNo10DrVy/W0rR5R08F2F074Kjxq5ESty/Mm79OSbgSRhQ\n"
                                                            "9cXO/8UJV0SqFL+kbjLP60yfXSbXQGafrv8A/t6ZIJPokJrE564m463UTo1TqUOj\n"
                                                            "3o155p5aFlHteX5QV4Gih2rOF/J84Kt/FVsDyR20XshB0XakrkEjAwgIQQKBgQCk\n"
                                                            "ZB9TAWRIihdO/UohOdxetA0kMN1m6QaQRYTQxSLNac5qifSLFYG4AucC4ttDOiFD\n"
                                                            "vFMHXB/FPRkrk1p4GoPMbrPhr+kcm+ShtP9ybHDBILPynu7LIduBTcTEdTo35pl/\n"
                                                            "eXUV9O+qmFFs0pZ7WVIsbhENb7J/Nx1L46UZmLuXgQKBgQDfMuU4K1jb8PCno5Uk\n"
                                                            "Dsrpp60GPyflskervoSqENy5yWNY7D2VEdFHRkz1BIRyTBXmsccrjPt+Co73aea2\n"
                                                            "n4CPCoQir7jkVTeKnZB8mZyZKOBYPGYeo46Bx4yTRvTZ9SuGmdtmvaTC6z1475Wx\n"
                                                            "zOnlXq5SzkJViJXh2CtNAthK4Q==\n"
                                                            "-----END PRIVATE KEY-----\n";

const unsigned char *key = (const unsigned char *)"-----BEGIN PUBLIC KEY-----\n"
                                                "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtuNBiX/VrhP71s+yTIp7\n"
                                                "pQbDkGfCQYJsa2Tw0Q4P/CaXiDVOa+fKf9KxH60L+MU24jGuP83gKA79DF+Hi098\n"
                                                "gLqCo5z8Tz4liNV4I78zbCUN99EZSFkr0lSF2/8kRToeBjIQ8ROxZQ3IkNbQqL+E\n"
                                                "OGgwD39prfOFBHiINhtYP01Huz8qeGtNUXqc2lo70vQxm6fRF+z+6K2CXXf1x6c8\n"
                                                "PMLvrE5adRM3z3+ZDwLuQ3iYGWIdcdc081Xs6UMzGgNf1ss8m6ZJzxElYqyXsCI6\n"
                                                "eU35Vr5+7QImOYDXnJi+1IiEHjaBiEypJh3dJk7m1oGLzR3FA2hVujoTXMWsqjmW\n"
                                                "pwIDAQAB\n"
                                                "-----END PUBLIC KEY-----\n";

#define MAIN_TASK_STACK_SIZE 8192

/* Number of retries for connection */
static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    /* Handle Wi-Fi and IP events */
    char* TAG = "event_handler"; // Declare and initialize TAG for logging purposes

    /* Wi-Fi event: Station started */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        /* Connect to Wi-Fi access point */
        esp_wifi_connect();
    } 
    /* Wi-Fi event: Station disconnected */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        /* Retry connecting to Wi-Fi access point if maximum retry count not reached */
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retry to connect to the AP");
        } 
        /* Set the Wi-Fi fail bit if maximum retry count reached */
        else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"Connect to the AP fail");
    } 
    /* IP event: Station got IP */
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        /* Get IP information from the event data */
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        /* Reset the retry count and set the Wi-Fi connected bit */
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


void wifi_init_sta(void)
{
    /* Initialize Wi-Fi station */
    char* TAG = "wifi_init_sta"; // Declare and initialize TAG for logging purposes

    /* Create event group for Wi-Fi events */
    s_wifi_event_group = xEventGroupCreate();

    /* Initialize the network interface */
    ESP_ERROR_CHECK(esp_netif_init());

    /* Create default event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Create default Wi-Fi station network interface */
    esp_netif_create_default_wifi_sta();

    /* Initialize Wi-Fi with default configuration */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Register event handlers for Wi-Fi and IP events */
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    /* Set Wi-Fi configuration */
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (password len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "Unexpected event!");
    }
}

void connect_tcp_socket(const char* brokerAddress, uint16_t port) 
{
    /* Connect to a TCP socket */
    char* TAG = "send_tcp_data"; // Declare and initialize TAG for logging purposes

    int addr_family = 0;
    int ip_protocol = 0;
    struct sockaddr_in dest_addr;

    /* Configure for IPv4 */
#if defined(CONFIG_EXAMPLE_IPV4)
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));   // Initialize hints structure with zeroes
    hints.ai_family = AF_INET;          // Specify address family as IPv4
    hints.ai_socktype = SOCK_STREAM;    // Specify socket type as stream
    hints.ai_protocol = IPPROTO_IP;     // Specify IP protocol

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port); // Convert port to string

    /* Resolve DNS name */
    int result = getaddrinfo(brokerAddress, port_str, &hints, &res); // Get address info of the broker
    if (result != 0) {
        ESP_LOGI(TAG, "Could not resolve DNS name..."); // Log if DNS name resolution failed
        return;
    }

    memcpy(&dest_addr, res->ai_addr, res->ai_addrlen); // Copy resolved address to destination address structure
    addr_family = AF_INET; // Set address family to IPv4
    ip_protocol = IPPROTO_IP; // Set IP protocol
    freeaddrinfo(res); // Free the memory allocated for address info
#endif

    /* Create the socket */
    sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGI(TAG, "Unable to create socket: errno %d", errno); // Log if socket creation failed
        return;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", brokerAddress, port); // Log socket creation success

    /* Connect to the socket */
    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGI(TAG, "Socket unable to connect: errno %d", errno); // Log if socket connection failed
        close(sock); // Close the socket
        return;
    }
    ESP_LOGI(TAG, "Socket successfully connected"); // Log socket connection success
}


void send_tcp_data(const char* data, uint16_t size)
{
    /* Send data over TCP connection */
    char* TAG = "send_tcp_data";                            // Declare and initialize TAG for logging purposes

    int err = send(sock, data, size, 0);                    // Send data through the socket

    // Check if data was sent successfully
    if (err < 0) {
        ESP_LOGI(TAG, "Failed to send data...");      // Log if data sending failed
    } else {
        ESP_LOGI(TAG, "Succeeded sending data..");   // Log if data sending succeeded
    }
}


void receive_tcp_data(void)
{
    char* TAG = "receive_tcp_data"; // Declare and initialize TAG for logging purposes                                                  

    /* Set socket to non-blocking mode */
    if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK) < 0) {
        ESP_LOGI(TAG, "Cannot put socket in non-blocking mode");        // Log error if unable to set non-blocking mode
    }

    while(1) {
        char temp_buffer[10]; // Define a temporary buffer of length 10

        /* Receive data from the socket */
        int len = recv(sock, temp_buffer, sizeof(temp_buffer) - 1, 0);

        if (len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            ESP_LOGI(TAG, "No data to read");                               // Log if no data is available to read
            break;
        } else if (len < 0) {
            ESP_LOGI(TAG, "Error during reception");                        // Log if an error occurred during receiving
            break;
        } else {
            /* Concatenate the received data to the existing data in the bytes_to_receive */
            memcpy((void *)(bytes_to_receive + number_of_bytes_received), (const void *) temp_buffer, len);

            number_of_bytes_received += len;                                // Increment the number of bytes received

            ESP_LOGI(TAG, "Received bytes: %d ", len);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void debug_print(char* message) 
{
    /* Send debugging information */
    char* TAG = "debug_print";                  // Declare and initialize TAG for logging purposes
    ESP_LOGI(TAG, "%s", message);               // Log the input message with ESP_LOGI function
}

size_t rsa2048_encrypt(const char *text)
{
    char* TAG = "rsa2048_encrypt";              // Declare and initialize TAG for logging purposes
    // RNG (Random number generator init)
    int ret = 0;
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *personalization = "mgkegneljgnjlwgnjefdcmeg12313123dsggsd";

    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *)personalization,
                                strlen(personalization));
    if (ret != 0)
    {
        // ERROR HANDLING CODE FOR YOUR APP
    }

    // Creating rsa context + Importing pub key
    ret = 0;
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    /*
     * Read the RSA public key
     */

    if ((ret = mbedtls_pk_parse_public_key(&pk, key, strlen((const char *)key) + 1)) != 0)
    {

        ESP_LOGI(TAG," failed\n  ! mbedtls_pk_parse_public_key returned -0x%04x\n", -ret);
    };
    // Encrypting data
    const unsigned char *to_encrypt = (const unsigned char *)text;
    size_t to_encrypt_len = strlen((const char *)to_encrypt);

    // unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    size_t olen = 0;

    /*
     * Calculate the RSA encryption of the data.
     */
    ESP_LOGI(TAG,"\n  . Generating the encrypted value: \n");
    fflush(stdout);

    if ((ret = mbedtls_pk_encrypt(&pk, to_encrypt, to_encrypt_len,
                                  buf, &olen, sizeof(buf),
                                  mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        ESP_LOGI(TAG, " failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret);
    }

    for (size_t i = 0; i < olen; i++)
    {
        mbedtls_printf("%02X%s", buf[i],
                       (i + 1) % 16 == 0 ? "\r\n" : " ");
    }

    mbedtls_pk_free(&pk);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);

    return olen;
}

void decrypt_test(char* text, size_t length)
{
    char* TAG = "decrypt_test";              // Declare and initialize TAG for logging purposes

    // RNG (Random number generator init)
    int ret = 0;
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *personalization = "mgkegneljgnjlwgnjefdcmeg12313123dsggsd";

    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *)personalization,
                                strlen(personalization));
    if (ret != 0)
    {
        // ERROR HANDLING CODE FOR YOUR APP
    }
    ret = 0;
    mbedtls_pk_context pk;

    mbedtls_pk_init(&pk);

    /*
     * Read the RSA private key
     */
    if ((ret = mbedtls_pk_parse_key(&pk, private_key, strlen((const char *)private_key) + 1, NULL, 0, mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        ESP_LOGI(TAG, " failed\n  ! mbedtls_pk_parse_key returned -0x%04x\n", -ret);
    }

    unsigned char result[MBEDTLS_MPI_MAX_SIZE];
    size_t olen = 0;

    /*
     * Calculate the RSA encryption of the data.
     */

    ESP_LOGI(TAG, "\n  . Generating the decrypted value");
    fflush(stdout);
    // const char *text = "fnianPxs/09bx75ufVLWPeFF9kbGEIL3+TQqW2+ZoeMpdvVnkifFToAii92ODVBPOL0RzQPfxlJcN/nVY3K5fWNSGHM8TTwTgCqvUc0ia5L5YHI1YSgDKzx2QPZlu7tEd06sjW7txRacnhilRfjFPp0CYeLwxYVBlPmKIE7oqQHrc8sal3X9NSqgwO7+03TBeH3beNanMCqQBRk9t+Z80XApEBMcZQHZ0lb+Z0C6DOuY0elH/fOp1SGlXuzf+tgcv7+TzL5uVVFCBNyMonTwMEp+zbLjX2Ck1IHhp8JXi3ovVi8HNcKCOQx/fxX1qTSt2NulHTwP2urCQSZbGjnYuw==";
    const unsigned char *to_decrypt = (unsigned char *)text;
    if ((ret = mbedtls_pk_decrypt(&pk, to_decrypt, length, result, &olen, sizeof(result),
                                  mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
        ESP_LOGI(TAG, " failed\n  ! mbedtls_pk_decrypt returned -0x%04x\n", -ret);
    }

    ESP_LOGI(TAG, " SUCESS WITH DECRYPTING");

    if (olen < sizeof(result)) {
        result[olen] = '\0';
    } else {
        ESP_LOGI(TAG, "Decrypted data is too large for the result buffer");
        // Handle this error case as needed
    }

    ESP_LOGI(TAG, "Decrypted string: %s", (char *)result);
}

// Function prototype for the new main task
void new_main_task(void *pvParameter);
void new_main_task(void *pvParameter)
{
    size_t length = rsa2048_encrypt("HELLO");
    decrypt_test((char*)buf, length);
    vTaskDelete(NULL);
}

int find_substring_index(const char *substr, size_t substr_len) {
    if (substr_len > number_of_bytes_received) {
        return -1;
    }

    for (size_t i = 0; i <= number_of_bytes_received - substr_len; ++i) {
        bool match = true;
        for (size_t j = 0; j < substr_len; ++j) {
            if (bytes_to_receive[i + j] != substr[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return -1;
}

void process_buffer_data(void) 
{
    char* TAG = "process_buffer_data"; // Declare and initialize TAG for logging purposes  

    ESP_LOGI(TAG, "Here is the total length of the buffer so far: %d ", number_of_bytes_received);

    /* Send debugging information */
    // for (int i = 0; i < number_of_bytes_received; i++)
    // {
    //     printf("%c ", bytes_to_receive[i]);
    // }
    // printf("\n");

    char* str1 = "/topic/topic3";
    char* str2 = "END_MESSAGE";

    int index1 = find_substring_index(str1, strlen(str1));
    int index2 = find_substring_index(str2, strlen(str2));

    if ((index1 != -1) && (index2 != -1)) {
        printf("Index of str1: %d\n", index1);
        printf("Index of str2: %d\n", index2);

        for (int i = index1 + strlen(str1); i < index2; i++)
        {
            printf("%c ", bytes_to_receive[i]);
        }
        printf("\n");
    }

    number_of_bytes_received = 0;
}

void app_main(void)
{
    char* TAG = "app_main";  // Declare and initialize TAG for logging purposes

    /* ------ Initialize NVS ------- */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* ------ Initialize Wifi ------- */
    ESP_LOGI(TAG, "Initializing device as station and connecting to wifi...");
    wifi_init_sta();

    /* --- Set External Functions --- */
    MQTT311_SetConnectTCPSocket(connect_tcp_socket);
    MQTT311_SetSendToTCPSocket(send_tcp_data);
    MQTT311_SetReadFromTCPSocket(receive_tcp_data);
    MQTT311_SetPrint(debug_print);
    MQTT311_SetProcessBufferData(process_buffer_data);

    /* ---- Start FreeRTOS Tasks ---- */
    MQTT311_CreateMQTTFreeRTOSTasks();

    /* ---- Connect to MQTT Broker ---- */
    MQTT311_CreateClient("client_id_dado");
    MQTT311_EstablishConnectionToMQTTBroker("mqtt.eclipseprojects.io", 1883);
    MQTT311_SetUsernameAndPassword("", "");
    MQTT311_Connect(0xC2, 600, "", "");

    /* ----- Publish some messages ------*/
    MQTT311_Publish(0x00, "/topic/topic1", 0x00, "123test");
    vTaskDelay(pdMS_TO_TICKS(1000));
    MQTT311_Publish(0x00, "/topic/topic2", 0x00, "Test123");
    vTaskDelay(pdMS_TO_TICKS(10000));
   
    /* ------ Subscribe to some topic ------ */
    MQTT311_Subscribe(0x02, "/topic/topic3", 0x00);
    vTaskDelay(pdMS_TO_TICKS(5000));
    // MQTT311_Publish(0x00, "/topic/topic3", 0x00, PGP_PUBLIC_KEY);

    /* ----- Unsubscribe to some topic ----- */
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // MQTT311_Unsubscribe(0x02, "/topic/mihajlo");

    /* ----- Test pinging ------ */
    MQTT311_Pingreq();

    /* ---- Test disconnecting ---- */
    // MQTT311_Disconnect();

    // Create the new main task with the custom stack size
    xTaskCreate(new_main_task, "new_main_task", MAIN_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}
