/**
 * @file OS_Ctrl.c
 * @author Matthew Cockburn 
 * @brief OS Ctrl Contains the allcation of the RTOS resources
 * including Task definitions, mutexs, queues, timers, etc
 * @version 1.0
 * @date 2023-09-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* Headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "System/ROS.h"
#include "Peripherals/usart.h"

/* Global Variables */
QueueHandle_t     ROSReaderQueue;
SemaphoreHandle_t ROSReaderSemphr;

/* Private Prototypes */
static void OS_HeartbeatTask(void);

/* Public Functions */
void OS_TaskInit(void){
    /* Added task to Kernel List */
	if(pdPASS != xTaskCreate((TaskFunction_t)ROS_ReaderTask,"ROS Reader",
			configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY+2,NULL)){
		return;
	}
    if(pdPASS != xTaskCreate((TaskFunction_t)OS_TaskInit,"OS Heartbeat",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY,NULL)){
        return;
    }
    return;
}

void OS_QueuesInit(void){
    ROSReaderQueue = xQueueCreate(MAX_USART_QUEUE_SIZE, sizeof(uint8_t));
    
    return;
}

void OS_SemaphoreInit(void){
    ROSReaderSemphr = xSemaphoreCreateBinary();
    xSemaphoreGive(ROSReaderSemphr);

    return;
}

void OS_MutexesInit(void){
    return;
}
/* End of Public Functions */

/* Private Functions */
static void OS_HeartbeatTask(void){
    while (1)
    {
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    return;
}
/* End of Private Functions */