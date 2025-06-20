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
#include <time.h>

typedef struct {
    char json[128]; // JSON 문자열을 저장할 버퍼
    long timestamp; // UNIX epoch seconds
} Message_t;

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
    xYaToYeQueue = xQueueCreate( 1, sizeof( Message_t ) );
    xYeToYaQueue = xQueueCreate( 1, sizeof( Message_t ) );

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
    Message_t msgToSend;
    Message_t msgReceived;

    for( ;; )
    {
        // 타임스탬프 저장
        msgToSend.timestamp = time(NULL);
        // JSON 메시지 작성 (timestamp 포함)
        snprintf(msgToSend.json, sizeof(msgToSend.json), "{\"from\":\"Ya\",\"msg\":\"Ping\",\"timestamp\":%ld}", msgToSend.timestamp);

        printf("TaskYa: Sending ping...\n");
        xQueueSend( xYaToYeQueue, &msgToSend, portMAX_DELAY );

        // 응답 대기
        if( xQueueReceive( xYeToYaQueue, &msgReceived, portMAX_DELAY ) == pdPASS )
        {
            printf("TaskYa: Received response: %s\n", msgReceived.json);
        }
        vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    }
}

/*----------------------------------------------------------*/

static void prvTaskYe( void *pvParameters )
{
    ( void ) pvParameters;
    Message_t msgToSend;
    Message_t msgReceived;

    for( ;; )
    {
        // 메시지 대기
        if( xQueueReceive( xYaToYeQueue, &msgReceived, portMAX_DELAY ) == pdPASS )
        {
            printf("TaskYe: Received message: %s\n", msgReceived.json);

            // 타임스탬프 저장
            msgToSend.timestamp = time(NULL);
            // JSON 메시지 작성 (timestamp 포함)
            snprintf(msgToSend.json, sizeof(msgToSend.json), "{\"from\":\"Ye\",\"msg\":\"Pong\",\"timestamp\":%ld}", msgToSend.timestamp);

            printf("TaskYe: Sending pong...\n");
            xQueueSend( xYeToYaQueue, &msgToSend, portMAX_DELAY );
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
