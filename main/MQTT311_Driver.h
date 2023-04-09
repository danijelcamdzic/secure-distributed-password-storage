/***********************************************************************
* FILENAME:        MQTT311_Driver.h             
*
* DESCRIPTION:
*                  Contains definitions, structures, variable and function declarations
*                  necessary for freeRTOS implementation of the MQTT 3.1.1 Client.
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

#ifndef MQTT311_DRIVER_H
#define MQTT311_DRIVER_H

/* Included libraries */
#include "MQTT311.h"        /* Include all the necessary MQTT311 Packets */

/* Priorities at which the tasks are created. */
// #define mqttQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
// #define mqttCHECK_SUB_MES_TASK_PRIORITY	    ( tskIDLE_PRIORITY + 2 )
// #define mqttDEBUG_TASK_PRIORITY		        ( tskIDLE_PRIORITY + 2 )

/* Time definitions */
// #define mainMQTT_SEND_FREQUENCY_MS          (100/portTICK_PERIOD_MS)
// #define mainMQTT_SUB_MES_FREQUENCY_MS       (100/portTICK_PERIOD_MS)
// #define mainDEBUG_FREQUENCY_MS              (100/portTICK_PERIOD_MS)

/* Task Handle */
// extern TaskHandle_t xMQTTSendTask;
// extern TaskHandle_t xMQTTCheckSubMesTask;
// extern TaskHandle_t xDebugTask;

/* Function declarations */
void create_mqtt_tasks(void);

#endif