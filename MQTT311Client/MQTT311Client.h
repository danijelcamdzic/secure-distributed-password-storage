/**
 * @file MQTT311Client.h
 * @brief Contains definitions necessary for the proper functioning of the
 * MQTT 3.1.1 implementation. Combines all the MQTT 3.1.1 packets and utilities.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_H
#define MQTT311Client_H

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
#include "MQTT311Client/MQTT311Client_Driver.h"
#include "MQTT311Client/MQTT311Client_Utilities.h"
#include "MQTT311Client/MQTT311Client_Connect.h"
#include "MQTT311Client/MQTT311Client_Connack.h"
#include "MQTT311Client/MQTT311Client_Publish.h"
#include "MQTT311Client/MQTT311Client_Puback.h"
#include "MQTT311Client/MQTT311Client_Pubrec.h"
#include "MQTT311Client/MQTT311Client_Pubrel.h"
#include "MQTT311Client/MQTT311Client_Pubcomp.h"
#include "MQTT311Client/MQTT311Client_Subscribe.h"
#include "MQTT311Client/MQTT311Client_Suback.h"
#include "MQTT311Client/MQTT311Client_Unsubscribe.h"
#include "MQTT311Client/MQTT311Client_Unsuback.h"
#include "MQTT311Client/MQTT311Client_Pingreq.h"
#include "MQTT311Client/MQTT311Client_Pingresp.h"
#include "MQTT311Client/MQTT311Client_Disconnect.h"

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

#endif /* MQTT311Client_H */
