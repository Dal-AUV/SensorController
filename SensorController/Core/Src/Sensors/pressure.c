/**
 * @file pressure.c
 * @author Matthew Cockburn 
 * @brief 
 * @version 1
 * @date 2023-09-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* Headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "System/OS_Ctrl.h"
#include "Peripherals/usart.h"
#include "System/ROS.h"
#include "Sensors/pressure.h"
/* Macros */
#define PRESSURE_TASK_DELAY_MS 200 // milliseconds
#define TEST_BUFFER 50
/* Data Structures */

/* Global Variables */

/* Private Prototypes */

/* RTOS Tasks */
void PRESSURE_Task(void * arguments){

    uint8_t byte_read = 0;
    char msg_str[TEST_BUFFER];
    char rpy_str[TEST_BUFFER];
    memset(msg_str,0,TEST_BUFFER);
    memset(rpy_str,0,TEST_BUFFER);
    while(1)
    {
        /* Send A Msg to the Pressure Sensor */
    	snprintf(msg_str, TEST_BUFFER, "\n\rPRESSURE MSG: 0x00,0x01,0x02\n\r");
        ROS_Write((uint8_t*)msg_str, TEST_BUFFER, portMAX_DELAY);

        /* Wait for data from the IMU */
        if(pdFAIL == xQueueReceive(PRESSURE_ReaderQueue,&byte_read,portMAX_DELAY)){
            continue;
        }

        /* Process the byte read in */
        snprintf(rpy_str,TEST_BUFFER,"\n\rPRESSURE Reply: %X\n\r",byte_read);
        ROS_Write((uint8_t*)rpy_str, TEST_BUFFER, portMAX_DELAY);

        /* Delay Until Next Loop */
        vTaskDelay(pdMS_TO_TICKS(PRESSURE_TASK_DELAY_MS));    
    }
}

void PRESSURE_TestTask(void * arguments){
    uint8_t byte = 0;
    while(1)
    {
        /* Generate a random byte value */
        byte = (uint8_t)rand();

        /* Send byte to the IMU Task */
        xQueueSendToBack(PRESSURE_ReaderQueue,&byte,portMAX_DELAY);

        /* Delay Until Next Loop*/
        vTaskDelay(pdMS_TO_TICKS(PRESSURE_TASK_DELAY_MS));
    }
}
/* Public Implementation */

/* Private Implementation */
