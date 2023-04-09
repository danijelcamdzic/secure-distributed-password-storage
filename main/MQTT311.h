/***********************************************************************
* FILENAME:        MQTT311.h             
*
* DESCRIPTION:
*                  Contains definitions necessary for the proper functioning of the 
*                  MQTT 3.1.1 implementation. Combines all the MQTT 3.1.1 packets and utilities.
*
* NOTES:
*       
*
* AUTHOR:          Danijel Camdzic     
*
*   
* DATE:            19 Aug 21
*
*
* CHANGES:
*
* VERSION:         DATE:          WHO:         DETAIL:
* 0.00.0           19 Aug 21      DC           Initial state of the file
*
*/

#ifndef MQTT311_H
#define MQTT311_H

/* Included libraries */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS library */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

/* MQTT Packet and Utilities Libraries */
#include "MQTT311_Utilities.h"
#include "MQTT311_Connect.h"
#include "MQTT311_Publish.h"

/* DELETE AFTER LIBRARY IS STANDALONE */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* MQTT Packet Types */
#define CONNECT                         0x10        /* Client request to connect to server*/
#define CONNACK                         0x20        /* Connect acknowledgment */
#define PUBLISH                         0x30        /* Publish message */
#define PUBACK                          0x40        /* Publish acknowledgement */
#define PUBREC                          0x50        /* Publish received (assured delivery part 1) */
#define PUBREL                          0x60        /* Publish release (assured delivery part 2) */
#define PUBCOMP                         0x70        /* Publish complete (assured delivery part 3) */
#define SUBSCRIBE                       0x80        /* Client subscribe request */
#define SUBACK                          0x90        /* Subscribe acknowledgement */
#define UNSUBSCRIBE                     0xA0        /* Unsubscribe request */
#define UNSUBACK                        0xB0        /* Unsubscribe acknowledgement */
#define PINGREQ                         0xC0        /* PING request */
#define PINGRESP                        0xD0        /* PING response */
#define DISCONNECT                      0xE0        /* Client is disconnecting */

/* MQTT Protocol Constants */
#define REMAINING_LENGTH_MAX            127         /* Maximum number for 1 byte representation of remaining length */

/* Bytes to read when the notification system displays 0 available bytes to read */
/* This is a precaution in case the system failed */
#define MAX_BYTES_TO_READ                   (500)

/* Uses macro to replace the pointer to the variable which holds the number of available bytes to read */
/* It is flexible - any variable which retrieves the number of bytes to read can be written here */
// #define AVAILABLE_BYTES                     (*bytes_to_read)
/* Uses macro to replace the pointer to the variable which holds the value of the signal quality */
// #define SIGNAL_MONITORING_VARIABLE          (*signal_monitoring_variable)

/* Signal strenght limit - 31 is the best quality, 99 is undetectable */
// #define HARD_RESET_LIMIT                    (2)     /* Restart is required and re-connection is required */

#endif