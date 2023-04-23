/**
 * @file debug_functions.c
 * @brief Contains helper debug functions implementation
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#include "main.h"
#include "MQTT311Client/MQTT311Client.h"
#include "RSA/RSA.h"
 
void debug_print(char* message) 
{
    /* Send debugging information */
    char* TAG = "debug_print";                  // Declare and initialize TAG for logging purposes
    ESP_LOGI(TAG, "%s", message);               // Log the input message with ESP_LOGI function
}