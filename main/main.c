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
#include "PgPKeys.h"

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


void receive_tcp_data(char* rx_buffer, uint16_t* number_of_bytes_received) 
{
    char* TAG = "receive_tcp_data"; // Declare and initialize TAG for logging purposes                                                         

    /* Set socket to non-blocking mode */
    if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK) < 0) {
        ESP_LOGI(TAG, "Cannot put socket in non-blocking mode");        // Log error if unable to set non-blocking mode
    }

    /* Receive data from the socket */
    int len = recv(sock, &rx_buffer[*number_of_bytes_received], sizeof(rx_buffer) - 1, 0);

    if (len < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        ESP_LOGI(TAG, "No data to read");                               // Log if no data is available to read
    } else if (len < 0) {
        ESP_LOGI(TAG, "Error during reception");                        // Log if an error occurred during receiving
    } else {
        *number_of_bytes_received += len;                               // Increment the number of bytes received
        ESP_LOGI(TAG, "Received %d bytes", *number_of_bytes_received);  // Log the number of bytes received
    }
}

void debug_print(char* message) 
{
    /* Send debugging information */
    char* TAG = "debug_print";                  // Declare and initialize TAG for logging purposes
    ESP_LOGI(TAG, "%s", message);               // Log the input message with ESP_LOGI function
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
    MQTT311_SetPrint(debug_print);
    MQTT311_SetConnectTCPSocket(connect_tcp_socket);
    MQTT311_SetSendToTCPSocket(send_tcp_data);
    MQTT311_SetReadFromTCPSocket(receive_tcp_data);

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
    vTaskDelay(pdMS_TO_TICKS(1000));

    MQTT311_Publish(0x00, "/topic/topic3", 0x00, PGP_PUBLIC_KEY);
    vTaskDelay(pdMS_TO_TICKS(1000));
   
    /* ------ Subscribe to some topic ------ */
    MQTT311_Subscribe(0x02, "/topic/topic4", 0x00);

    /* ----- Unsubscribe to some topic ----- */
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // MQTT311_Unsubscribe(0x02, "/topic/mihajlo");

    /* ----- Test pinging ------ */
    MQTT311_Pingreq();

    /* ---- Test disconnecting ---- */
    // MQTT311_Disconnect();
}
