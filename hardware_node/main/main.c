/* Secure Destributed Password Storage

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "main.h"
#include "tcp_functions.h"
#include "mqtt_functions.h"
#include "nvs_functions.h"
#include "MQTT311Client/MQTT311Client.h"
#include "RSA/RSA.h"

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

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

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

    /* ------ Initialize NVS ------- */
    ESP_ERROR_CHECK(nvs_init());

    /* --- Set External Functions --- */
    MQTT311Client_SetConnectTCPSocket(tcp_connect_socket);
    MQTT311Client_SetSendToTCPSocket(tcp_send_data);
    MQTT311Client_SetReadFromTCPSocket(tcp_receive_data);
    MQTT311Client_SetPrint(debug_print);
    MQTT311Client_SetProcessBufferData(mqtt_process_buffer_data);
    RSA_SetPrint(debug_print);

    /* ---- Start FreeRTOS Tasks ---- */
    MQTT311Client_CreateMQTTFreeRTOSTasks();

    /* ---- Connect to MQTT Broker ---- */
    MQTT311Client_CreateClient(CLIENT_ID);
    MQTT311Client_EstablishConnectionToMQTTBroker(BROKER_ADDRESS, BROKER_PORT_TCP);
    MQTT311Client_SetUsernameAndPassword("", "");
    MQTT311Client_Connect(0xC2, 600, "", "");

    /* ----- Publish some messages ------*/
    // MQTT311Client_Publish(0x00, "/topic/topic1", 0x00, "123test", sizeof("123test"));
    // vTaskDelay(pdMS_TO_TICKS(1000));
   
    /* ------ Subscribe to some topic ------ */
    MQTT311Client_Subscribe(0x02, SUB_TOPIC, 0x00);
    vTaskDelay(pdMS_TO_TICKS(1000));
    MQTT311Client_Subscribe(0x02, ALL_TOPIC, 0x00);

    /* ----- Unsubscribe to some topic ----- */
    vTaskDelay(pdMS_TO_TICKS(1000));
    MQTT311Client_Unsubscribe(0x02, "/topic/unsubscribe_test");

    /* ----- Test pinging ------ */
    MQTT311Client_Pingreq();

    /* ---- Test disconnecting ---- */
    // MQTT311Client_Disconnect();
}
