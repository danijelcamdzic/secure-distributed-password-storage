/**
 * @file MQTT311Client_Driver.c
 * @brief Contains freeRTOS implementation of the MQTT 3.1.1 Client.
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* Included libraries */
#include "MQTT311Client/MQTT311Client_Driver.h"    
#include "MQTT311Client/MQTT311Client_Pingreq.h"

/* Variable definitions */
#define PING_TIME                           pdMS_TO_TICKS(userdata.keepAlive*1000*0.7)

/* Task handle */
TaskHandle_t xMQTTSendTask = NULL;
TaskHandle_t xMQTTCheckSubMesTask = NULL;

/* Sempahore Handle */
SemaphoreHandle_t xMQTTSemaphore = NULL;

/* Task prototype */
static void prvMQTTQueueSendTask( void *pvParameters );
static void prvMQTTCheckSubMesTask( void *pvParameters );

/**
 * @brief Sends "send" type MQTT packets with the correct response.
 *
 * This function is a task that sends "send" type MQTT packets with the correct response within.
 *
 * @param pvParameters pointer to task parameters.
 *
 * @return None.
 */
static void prvMQTTQueueSendTask( void *pvParameters )
{
    /* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

    TickType_t xNextWakeTime;
    BaseType_t xStatus;

    /* Variable that receives MQTT "send" packet types */
    struct MQTTPacket mqtt_packet;

    xNextWakeTime = xTaskGetTickCount();

    /* Initialize the last sent time */
    TickType_t xLastSentTime = xTaskGetTickCount();

    for( ;; )
	{     
        if ( xMQTTSemaphore != NULL )
        {
            /* Try to take the sempahore */
            if( xSemaphoreTake( xMQTTSemaphore, ( TickType_t ) 10 ) == pdTRUE )
            {
                /* Wait 1000ms for something to be received from the queue */
                xStatus = xQueueReceive(xMQTTQueue, &mqtt_packet, pdMS_TO_TICKS(100));
                if (xStatus == pdTRUE)
                {
                    /* Send appropriate packet */
                    MQTT311Client_SendMQTTPacket(&mqtt_packet);

                    /* Update the last sent time */
                    xLastSentTime = xTaskGetTickCount();
                }
                /* Give sempahore back and delay the task */
                xSemaphoreGive( xMQTTSemaphore );
                /* Delay the task for some time */
                vTaskDelayUntil( &xNextWakeTime, mainMQTT_SEND_FREQUENCY_MS );
            }
        }
        else
        {
            /* Do nothing */
        }

        /* Check if SET_TIME has passed since the last packet was sent */
        if( ( xTaskGetTickCount() - xLastSentTime ) >= PING_TIME )
        {
            /* Call the Pingreq function */
            MQTT311Client_Pingreq();

            /* Update the last sent time */
            xLastSentTime = xTaskGetTickCount();
        }
    }
}

/**
 * @brief Checks for pending MQTT messages on the socket and reads the required bytes.
 *
 * This function checks if there are any messages that need to be read from the socket and reads the correct
 * amount of bytes if any messages are available. If no messages are available, a defined number of bytes is read
 * in case the byte notification system lagged or didn't deliver correct information.
 *
 * @param pvParameters pointer to task parameters.
 *
 * @return None.
 */
static void prvMQTTCheckSubMesTask( void *pvParameters )
{
    /* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
	{     
        if ( xMQTTSemaphore != NULL )
        {
            /* Try to take the sempahore */
            if( xSemaphoreTake( xMQTTSemaphore, portMAX_DELAY ) == pdTRUE )
            {
                MQTT311Client_ReceiveFromMQTTBroker();
                /* Process the buffer data */
                MQTT311Client_ProcessBufferData();
                /* Give back the semaphore (unlock) */
                xSemaphoreGive( xMQTTSemaphore );
                /* Delay the task */
                vTaskDelayUntil( &xNextWakeTime, mainMQTT_SEND_FREQUENCY_MS );
            }
        }
        else
        {
            /* Do nothing */
        }
    }
}

/**
 * @brief Creates MQTT tasks and other FreeRTOS objects.
 *
 * This function creates MQTT tasks and other FreeRTOS objects required for MQTT operation.
 *
 * @return None.
 */
void MQTT311Client_CreateMQTTFreeRTOSTasks(void)
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
    xTaskCreate( prvMQTTQueueSendTask, "MQTTTx", 8*configMINIMAL_STACK_SIZE, NULL, mqttQUEUE_SEND_TASK_PRIORITY, &xMQTTSendTask );
    xTaskCreate( prvMQTTCheckSubMesTask, "MQTTSubMes", 8*configMINIMAL_STACK_SIZE, NULL, mqttCHECK_SUB_MES_TASK_PRIORITY, &xMQTTCheckSubMesTask );
}

