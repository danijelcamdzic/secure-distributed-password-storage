/**
 * @file main.c
 * @brief Includes the wifi connection function and app_main task which
 * connects to the MQTT broker and sets up the external functions
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
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

/* Utility function headers */
#include "tcp_functions.h"
#include "ssl_functions.h"
#include "mqtt_functions.h"
#include "nvs_functions.h"

/* MQTT library headers */
#include "MQTT311Client/MQTT311Client.h"
#include "MQTT311Client/MQTT311Client_Driver.h"
#include "MQTT311Client/MQTT311Client_Connect.h"
#include "MQTT311Client/MQTT311Client_Subscribe.h"

/* RSA utility library header */
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

/**
 * @brief Event handler for Wi-Fi and IP events.
 *
 * This function handles Wi-Fi and IP events in the system, such as
 * connecting, disconnecting, and obtaining an IP address.
 *
 * @param arg         Unused argument.
 * @param event_base  The event base indicating the type of event (Wi-Fi or IP).
 * @param event_id    The event ID associated with the event being handled.
 * @param event_data  Pointer to the event data.
 *
 * The function handles the following events:
 * - WIFI_EVENT_STA_START: Connects to the Wi-Fi access point.
 * - WIFI_EVENT_STA_DISCONNECTED: Retries connecting to the Wi-Fi access point if the maximum
 *   retry count is not reached. Sets the Wi-Fi fail bit if the maximum retry count is reached.
 * - IP_EVENT_STA_GOT_IP: Logs the assigned IP address, resets the retry count, and sets the
 *   Wi-Fi connected bit.
 */
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

/**
 * @brief Initialize and start the Wi-Fi station.
 *
 * This function initializes the Wi-Fi station, configures the network interface,
 * registers event handlers, sets the Wi-Fi configuration, and starts the Wi-Fi station.
 * It then waits for the connection to be established or for the maximum number of retries
 * to be reached.
 *
 * The following events are handled by the event_handler (see event_handler() function):
 * - WIFI_CONNECTED_BIT: Connection established.
 * - WIFI_FAIL_BIT: Connection failed after reaching the maximum number of retries.
 *
 * Upon successful connection, the function logs the SSID and password.
 * If the connection fails, it logs the SSID and password along with the failure message.
 * In case of an unexpected event, an error message is logged.
 */
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

/**
 * @brief Print a debug message using the ESP_LOGI function.
 *
 * This function sends debugging information by logging the input message with
 * the ESP_LOGI function, which is part of the ESP-IDF logging library.
 *
 * @param message  The debug message to be logged.
 */
void debug_print(char* message) 
{
    /* Send debugging information */
    char* TAG = "debug_print";                  /**< Declare and initialize TAG for logging purposes */
    ESP_LOGI(TAG, "%s", message);               /**< Log the input message with ESP_LOGI function */
}

/**
 * @brief Main application entry point.
 *
 * This function initializes the device as a Wi-Fi station, connects to a Wi-Fi network,
 * sets external functions for the MQTT library and RSA utility library, creates FreeRTOS tasks
 * for sending and receiving MQTT messages, connects to an MQTT broker, and subscribes to
 * MQTT topics.
 */
void app_main(void)
{
    char* TAG = "app_main";                     /**< Declare and initialize TAG for logging purposes */

    /* Initialize NVS */
    ESP_ERROR_CHECK(nvs_init());

    /* Initialize Wi-Fi */
    ESP_LOGI(TAG, "Initializing device as station and connecting to wifi...");
    wifi_init_sta();

    /* Set external functions for MQTT library */
    MQTT311Client_SetConnectTCPSocket(ssl_connect_socket);
    MQTT311Client_SetSendToTCPSocket(ssl_send_data);
    MQTT311Client_SetReadFromTCPSocket(ssl_receive_data);
    MQTT311Client_SetPrint(debug_print);
    MQTT311Client_SetProcessBufferData(mqtt_process_buffer_data);

    /* Set external functions for RSA utility library */
    RSA_SetPrint(debug_print);

    /* Start FreeRTOS tasks for sending and receiving MQTT messages */
    MQTT311Client_CreateMQTTFreeRTOSTasks();

    /* Connect to MQTT broker */
    MQTT311Client_CreateClient(CLIENT_ID);
    MQTT311Client_EstablishConnectionToMQTTBroker(BROKER_ADDRESS, BROKER_PORT_SSL);
    MQTT311Client_SetUsernameAndPassword("", "");
    MQTT311Client_Connect(0xC2, KEEP_ALIVE, "", "");
   
    /* Subscribe to MQTT topics */
    MQTT311Client_Subscribe(0x02, SUB_TOPIC, 0x00);
    vTaskDelay(pdMS_TO_TICKS(1000));
    MQTT311Client_Subscribe(0x02, ALL_TOPIC, 0x00);
    vTaskDelay(pdMS_TO_TICKS(1000));
}
