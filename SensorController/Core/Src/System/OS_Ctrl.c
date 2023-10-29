/**
 * @file OS_Ctrl.c
 * @author Matthew Cockburn 
 * @brief OS Ctrl Contains the allocation of the RTOS resources
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
#include "build.h"
#include "System/ROS.h"
#include "System/OS_Ctrl.h"
#include "Sensors/imu.h"
#include "Sensors/pressure.h"
#include "Interfaces/ros_if.h"
#include "Peripherals/usart.h"

/* Global Variables */
QueueHandle_t     ROS_ReaderQueue;
QueueHandle_t 	  ROS_WriterQueue;
QueueHandle_t     IMU_ReaderQueue;
QueueHandle_t     PRESSURE_ReaderQueue;

SemaphoreHandle_t ROS_WriterSem;


#define TEST_IMU
/* Private Prototypes */
static void OS_HeartbeatTask(void);

/* Public Functions */
void OS_TasksInit(void){
    /* Added task to Kernel List */

	if(pdPASS != xTaskCreate((TaskFunction_t)ROS_ReaderTask,"ROS Reader",
		ROS_READER_STACK_SIZE, NULL, ROS_READER_PRI, NULL)){
        return;
	}

    if(pdPASS != xTaskCreate((TaskFunction_t)ROS_WriterTask,"ROS Writer",
        ROS_WRITER_STACK_SIZE, NULL, ROS_WRITER_PRI, NULL)){
        return;
	}

    #ifdef TEST_IMU
    if(pdPASS != xTaskCreate((TaskFunction_t)IMU_Task,"IMU Task",
        IMU_STACK_SIZE, NULL, IMU_PRI, NULL)){
        return;
	}
    if(pdPASS != xTaskCreate((TaskFunction_t)IMU_TestTask,"IMU Test Task",
        IMU_STACK_SIZE, NULL, IMU_PRI, NULL)){
        return;
	}
    #endif
    #ifdef TEST_PRESSURE
    if(pdPASS != xTaskCreate((TaskFunction_t)PRESSURE_Task,"Pressure Task",
        PRESSURE_STACK_SIZE, NULL, PRESSURE_PRI, NULL)){
        return;
	}
    if(pdPASS != xTaskCreate((TaskFunction_t)PRESSURE_TestTask,"Pressure Test Task",
        PRESSURE_STACK_SIZE, NULL, PRESSURE_PRI, NULL)){
        return;
	}
    #endif
    if(pdPASS != xTaskCreate((TaskFunction_t)OS_HeartbeatTask,"OS Heartbeat",
            configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY,NULL)){
        return;
    }
    return;
}

void OS_QueuesInit(void){
    ROS_ReaderQueue       = xQueueCreate(MAX_USART_QUEUE_SIZE, sizeof(uint8_t));
    ROS_WriterQueue		  = xQueueCreate(ROS_WRITER_QUEUE_LEN, sizeof(ROS_Transport_t));
    IMU_ReaderQueue       = xQueueCreate(IMU_QUEUE_SIZE, sizeof(uint8_t));
    PRESSURE_ReaderQueue  = xQueueCreate(PRESSURE_QUEUE_SIZE, sizeof(uint8_t));
    return;
}

void OS_SemaphoreInit(void){
    ROS_WriterSem = xSemaphoreCreateBinary();
    xSemaphoreGive(ROS_WriterSem);
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
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    return;
}
/* End of Private Functions */
