/***********************************************************************
* FILENAME:        MQTT311_Driver.c             
*
* DESCRIPTION:
*                  Contains freeRTOS implementation of the MQTT 3.1.1 Client.
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

/* Included libraries */
#include "MQTT311_Driver.h"    

/* Variable definitions */

/* Task handle */
// TaskHandle_t xMQTTSendTask = NULL;
// TaskHandle_t xMQTTCheckSubMesTask = NULL;
// TaskHandle_t xDebugTask = NULL;

/* Sempahore Handle */
// SemaphoreHandle_t xMQTTSemaphore = NULL;

/* Task prototype */
// static void prvMQTTQueueSendTask( void *pvParameters );
// static void prvMQTTCheckSubMesTask( void *pvParameters );
// static void prvDebugTask( void *pvParameters );

/*
 * Function: prvMQTTQueueSendTask
 * ----------------------------
 *   Tasks that sends "send" type MQTT packets and with the correct response within.
 *
 *   returns: no return value
 */
// static void prvMQTTQueueSendTask( void *pvParameters )
// {
//     TickType_t xNextWakeTime;

//     /* Variable that receives MQTT "send" packet types */
//     struct MQTTPacket mqtt_packet;
 
// 	/* Remove compiler warning about unused parameter. */
// 	( void ) pvParameters;
//     xNextWakeTime = xTaskGetTickCount();

//     for( ;; )
// 	{     
//         if ( xMQTTSemaphore != NULL )
//         {
//             /* Try to take the sempahore */
//             if( xSemaphoreTake( xMQTTSemaphore, ( TickType_t ) 10 ) == pdTRUE )
//             {
//                 /* Wait until something is received from the queue */
//                 xQueueReceive( xMQTTQueue, &mqtt_packet, portMAX_DELAY );
//                 /* Send appropriate packet */
//                 send_mqtt_packet(&mqtt_packet);
//                 /* Give sempahore back and delay the task */
//                 xSemaphoreGive( xMQTTSemaphore );
//                 /* Delay the task for some time */
//                 vTaskDelayUntil( &xNextWakeTime, mainMQTT_SEND_FREQUENCY_MS );
//             }
//         }
//         else
//         {
//             printf("\r\nSemaphore is NULL\r\n");
//         }
//     }
// }

/*
 * Function: prvMQTTCheckSubMesTask
 * ----------------------------
 *   Checks if any messages that need to be read from the socket exist and reads
 *   the correct ammount of bytes if there are. If not, a defined number of bytes is read
 *   in case the byte notification system lagged or didn't deliver correct information 
 *
 *   returns: no return value
 */
// static void prvMQTTCheckSubMesTask( void *pvParameters )
// {
//     TickType_t xNextWakeTime;
 
// 	/* Remove compiler warning about unused parameter. */
// 	( void ) pvParameters;
//     xNextWakeTime = xTaskGetTickCount();

//     for( ;; )
// 	{     
//         if ( xMQTTSemaphore != NULL )
//         {
//             /* Try to take the sempahore */
//             if( xSemaphoreTake( xMQTTSemaphore, portMAX_DELAY ) == pdTRUE )
//             {
//                 /* If there are no bytes to be read (==0), the system could be malfunctioning */
//                 /* so a fixed number of bytes is read from the socket at each turn */
//                 if (AVAILABLE_BYTES == 0)
//                 {
//                     AVAILABLE_BYTES = MAX_BYTES_TO_READ;
//                 }
//                 /* Read number of bytes specified by the variable using the connected read function */
//                 char bytes_num_str[4];
//                 sprintf(bytes_num_str, "%d", AVAILABLE_BYTES);
//                 mqtt_ext_functions.read_data_from_socket(userdata.socketID, bytes_num_str);
//                 /* Set the variable to 0 */
//                 AVAILABLE_BYTES = 0;

//                 /* Cheecking the signal strength */
//                 mqtt_ext_functions.monitor_connection();
//                 /* Reset if signal is too low */
//                 if ((SIGNAL_MONITORING_VARIABLE < HARD_RESET_LIMIT) || (SIGNAL_MONITORING_VARIABLE == 99))
//                 {
//                     /* Hard reset and re-connect */
//                     printf("\r\n Detected low signal... Reconnecting...\r\n");
//                     xQueueReset(xMQTTQueue);
//                     //reconnection_sequence();
//                 }
                
//                 /* Give back the semaphore (unlock) */
//                 xSemaphoreGive( xMQTTSemaphore );
//                 /* Delay the task */
//                 vTaskDelayUntil( &xNextWakeTime, mainMQTT_SEND_FREQUENCY_MS );
//             }
//         }
//         else
//         {
//             printf("\r\nSemaphore is NULL\r\n");
//         }
//     }
// }

/*
 * Function: prvDebugTask
 * ----------------------------
 *   Tasks that helps debug.
 *
 *   returns: no return value
 */
// static void prvDebugTask( void *pvParameters )
// {
//     TickType_t xNextWakeTime;
 
// 	/* Remove compiler warning about unused parameter. */
// 	( void ) pvParameters;
//     xNextWakeTime = xTaskGetTickCount();

//     for( ;; )
// 	{     
//         /* Additional tasks to be implemented */
//     }
// }

/*
 * Function: create_mqtt_tasks
 * ----------------------------
 *   Creates MQTT tasks and other freeRTOS objects.
 *
 *   returns: no return value
 */
void create_mqtt_tasks(void)
{
    /* Setting the user structure fields to NULL */
    userdata.brokerAddress = NULL;
    userdata.socketID = NULL;
    userdata.username = NULL;
    userdata.password = NULL;
    userdata.deviceID = NULL;

    /* Create the queue. */
    xMQTTQueue = xQueueCreate(mqttQUEUE_LENGTH, sizeof(struct MQTTPacket));
    NULL_CHECK(xMQTTQueue)
    
    /* Create a semaphore */
    // xMQTTSemaphore = xSemaphoreCreateBinary();
    // xSemaphoreGive( xMQTTSemaphore );
    // NULL_CHECK(xMQTTSemaphore)

    /* Creating a task */
    // xTaskCreate( prvMQTTQueueSendTask, "MQTTTx", configMINIMAL_STACK_SIZE, NULL, mqttQUEUE_SEND_TASK_PRIORITY, &xMQTTSendTask );
    // xTaskCreate( prvMQTTCheckSubMesTask, "MQTTSubMes", configMINIMAL_STACK_SIZE, NULL, mqttCHECK_SUB_MES_TASK_PRIORITY, &xMQTTCheckSubMesTask );
    //xTaskCreate( prvDebugTask, "DebugTask", configMINIMAL_STACK_SIZE, NULL, mqttDEBUG_TASK_PRIORITY, &xDebugTask );
}

