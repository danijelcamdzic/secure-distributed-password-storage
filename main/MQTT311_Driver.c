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

static const char *TAG = "MQTT311_Driver";

/* Task handle */
TaskHandle_t xMQTTSendTask = NULL;
TaskHandle_t xMQTTCheckSubMesTask = NULL;

/* Sempahore Handle */
SemaphoreHandle_t xMQTTSemaphore = NULL;

/* Task prototype */
static void prvMQTTQueueSendTask( void *pvParameters );
static void prvMQTTCheckSubMesTask( void *pvParameters );

/*
 * Function: prvMQTTQueueSendTask
 * ----------------------------
 *   Tasks that sends "send" type MQTT packets and with the correct response within.
 *
 *   returns: no return value
 */
static void prvMQTTQueueSendTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    BaseType_t xStatus;

    /* Variable that receives MQTT "send" packet types */
    struct MQTTPacket mqtt_packet;
 
	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
	{     
        if ( xMQTTSemaphore != NULL )
        {
            /* Try to take the sempahore */
            if( xSemaphoreTake( xMQTTSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
                /* Wait until something is received from the queue */
                xStatus = xQueueReceive(xMQTTQueue, &mqtt_packet, pdMS_TO_TICKS(1000));
                if (xStatus == pdTRUE)
                {
                    /* Send appropriate packet */
                    MQTT311_SendMQTTPacket(&mqtt_packet);
                }
                /* Give sempahore back and delay the task */
                xSemaphoreGive( xMQTTSemaphore );
                /* Delay the task for some time */
                vTaskDelayUntil( &xNextWakeTime, mainMQTT_SEND_FREQUENCY_MS );
            }
        }
        else
        {
        }
    }
}

/*
 * Function: prvMQTTCheckSubMesTask
 * ----------------------------
 *   Checks if any messages that need to be read from the socket exist and reads
 *   the correct ammount of bytes if there are. If not, a defined number of bytes is read
 *   in case the byte notification system lagged or didn't deliver correct information 
 *
 *   returns: no return value
 */
static void prvMQTTCheckSubMesTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
 
	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
	{     
        if ( xMQTTSemaphore != NULL )
        {
            /* Try to take the sempahore */
            if( xSemaphoreTake( xMQTTSemaphore, portMAX_DELAY ) == pdTRUE )
            {
                MQTT311_ReceiveFromMQTTBroker();
                if (number_of_bytes_received > 50) 
                {
                    for (int i = 0; i < number_of_bytes_received; i++)
                    {
                        ESP_LOGI("MESSAGE", "%c ", bytes_to_receive[i]);
                    }
                    number_of_bytes_received = 0;
                }
                /* Give back the semaphore (unlock) */
                xSemaphoreGive( xMQTTSemaphore );
                /* Delay the task */
                vTaskDelayUntil( &xNextWakeTime, mainMQTT_SEND_FREQUENCY_MS );
            }
        }
        else
        {
        }
    }
}

/*
 * Function: MQTT311_CreateMQTTFreeRTOSTasks
 * ----------------------------
 *   Creates MQTT tasks and other freeRTOS objects.
 *
 *   returns: no return value
 */
void MQTT311_CreateMQTTFreeRTOSTasks(void)
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
    xMQTTSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive( xMQTTSemaphore );
    NULL_CHECK(xMQTTSemaphore)

    /* Creating a task */
    xTaskCreate( prvMQTTQueueSendTask, "MQTTTx", 4*configMINIMAL_STACK_SIZE, NULL, mqttQUEUE_SEND_TASK_PRIORITY, &xMQTTSendTask );
    xTaskCreate( prvMQTTCheckSubMesTask, "MQTTSubMes", 4*configMINIMAL_STACK_SIZE, NULL, mqttCHECK_SUB_MES_TASK_PRIORITY, &xMQTTCheckSubMesTask );
}

