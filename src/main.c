#include <stdio.h>
#include <pthread.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Priorities for the tasks */
#define mainTASK_YA_PRIORITY    ( tskIDLE_PRIORITY + 1 )
#define mainTASK_YE_PRIORITY    ( tskIDLE_PRIORITY + 2 )

/* Queues used by the tasks */
static QueueHandle_t xYaToYeQueue = NULL;
static QueueHandle_t xYeToYaQueue = NULL;

/* Task functions */
static void prvTaskYa( void *pvParameters );
static void prvTaskYe( void *pvParameters );

/*----------------------------------------------------------*/

int main( void )
{
    printf("Starting myTaskApp FreeRTOS Demo\n");

    /* Create the queues used by TaskYa and TaskYe. */
    xYaToYeQueue = xQueueCreate( 1, sizeof( char * ) );
    xYeToYaQueue = xQueueCreate( 1, sizeof( char * ) );

    if( ( xYaToYeQueue != NULL ) && ( xYeToYaQueue != NULL ) )
    {
        /* Create the tasks. */
        xTaskCreate( prvTaskYa, "TaskYa", configMINIMAL_STACK_SIZE, NULL, mainTASK_YA_PRIORITY, NULL );
        xTaskCreate( prvTaskYe, "TaskYe", configMINIMAL_STACK_SIZE, NULL, mainTASK_YE_PRIORITY, NULL );

        /* Start the scheduler. */
        vTaskStartScheduler();
    }

    /* The scheduler will run forever, so the following line should never be reached. */
    for( ;; );

    return 0;
}

/*----------------------------------------------------------*/

static void prvTaskYa( void *pvParameters )
{
    ( void ) pvParameters;
    const char *pcMessageToSend = "Ping from Ya";
    char *pcReceivedMessage;

    for( ;; )
    {
        printf("TaskYa: Sending ping...\n");
        xQueueSend( xYaToYeQueue, &pcMessageToSend, portMAX_DELAY );

        if( xQueueReceive( xYeToYaQueue, &pcReceivedMessage, portMAX_DELAY ) == pdPASS )
        {
            printf("TaskYa: Received response: \"%s\"\n", pcReceivedMessage);
        }

        vTaskDelay( pdMS_TO_TICKS( 2000 ) );
    }
}

/*----------------------------------------------------------*/

static void prvTaskYe( void *pvParameters )
{
    ( void ) pvParameters;
    char *pcReceivedMessage;
    const char *pcMessageToSend = "Pong from Ye";

    for( ;; )
    {
        if( xQueueReceive( xYaToYeQueue, &pcReceivedMessage, portMAX_DELAY ) == pdPASS )
        {
            printf("TaskYe: Received message: \"%s\"\n", pcReceivedMessage);
            printf("TaskYe: Sending pong...\n");
            xQueueSend( xYeToYaQueue, &pcMessageToSend, portMAX_DELAY );
        }
    }
}

/*----------------------------------------------------------*/

/* Needed for some configurations, but not used in this simple demo. */
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

void vApplicationIdleHook( void )
{
    /* Best practice to sleep the CPU here to save power, but for this demo, we do nothing. */
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    taskDISABLE_INTERRUPTS();
    for( ;; );
}
