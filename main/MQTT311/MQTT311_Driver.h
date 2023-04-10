/**
 * @file MQTT311_Driver.h
 * @brief Contains definitions, structures, variable and function declarations
 * necessary for freeRTOS implementation of the MQTT 3.1.1 Client.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311_DRIVER_H
#define MQTT311_DRIVER_H

/* Included libraries */
#include "MQTT311/MQTT311.h"        /* Include all the necessary MQTT311 Packets */

/* Priorities at which the tasks are created. */
#define mqttQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mqttCHECK_SUB_MES_TASK_PRIORITY	    ( tskIDLE_PRIORITY + 2 )

/* Time definitions */
#define mainMQTT_SEND_FREQUENCY_MS          (100/portTICK_PERIOD_MS)
#define mainMQTT_SUB_MES_FREQUENCY_MS       (100/portTICK_PERIOD_MS)

/* Task Handle */
extern TaskHandle_t xMQTTSendTask;
extern TaskHandle_t xMQTTCheckSubMesTask;

/* Function declarations */
void MQTT311_CreateMQTTFreeRTOSTasks(void);

#endif /* MQTT311_DRIVER_H */
