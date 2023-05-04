/**
 * @file MQTT311Client_Driver.h
 * @brief Contains definitions, structures, variable and function declarations
 * necessary for freeRTOS implementation of the MQTT 3.1.1 Client.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#ifndef MQTT311Client_DRIVER_H
#define MQTT311Client_DRIVER_H

/* Included libraries */
#include "MQTT311Client/MQTT311Client.h"        /**< Include all the necessary MQTT311 Packets */

/* Priorities at which the tasks are created. */
#define mqttQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mqttCHECK_SUB_MES_TASK_PRIORITY	    ( tskIDLE_PRIORITY + 2 )

/* Time definitions */
#define mainMQTT_SEND_FREQUENCY_MS          (1000/portTICK_PERIOD_MS)
#define mainMQTT_SUB_MES_FREQUENCY_MS       (1000/portTICK_PERIOD_MS)

/* Task Handle */
extern TaskHandle_t xMQTTSendTask;
extern TaskHandle_t xMQTTCheckSubMesTask;

/* Ping timing define */
#define PING_TIME                           pdMS_TO_TICKS(userdata.keepAlive*1000*0.6) /**< 60% of time defined in the keep alive interval */

/* Function declarations */
void MQTT311Client_CreateMQTTFreeRTOSTasks(void);

#endif /* MQTT311Client_DRIVER_H */

