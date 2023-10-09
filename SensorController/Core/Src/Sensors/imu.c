/**
 * @file imu.c
 * @author Matthew Cockburn 
 * @brief IMU Sensor C Source File
 * @version 1
 * @date 2023-09-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* Headers */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Peripherals/usart.h"
#include "System/OS_Ctrl.h"
#include "Sensors/imu.h"
#include "Peripherals/i2c.h"

/* Macros */
#define IMU_TASK_DELAY_MS 1000 // milliseconds
#define TEST_BUFFER 50
/* Testing Defines*/

#define TEMP_TASK
//#define SAMPLE_CODE

/* Data Structures */

/* Global Variables */

/* Private Prototypes */
//void IMU_Task(LSM6DS3 * sensor);
/* RTOS Tasks */
void IMU_Task(LSM6DS3 * sensor){

    uint8_t byte_read = 0;
    char msg_str[TEST_BUFFER];
    char rpy_str[TEST_BUFFER];
    LSM6DS3_Init(sensor,&hi2c1);


    memset(msg_str,0,TEST_BUFFER);
    memset(rpy_str,0,TEST_BUFFER);
    while(1)
    {
#ifdef SAMPLE_CODE
        /* Send A Msg to the IMU */
    	snprintf(msg_str, TEST_BUFFER, "\n\rIMU MSG: 0x00,0x01,0x02\n\r");
        ROS_Write((uint8_t*)msg_str, TEST_BUFFER, portMAX_DELAY);

        /* Wait for data from the IMU */
        if(pdFAIL == xQueueReceive(IMU_ReaderQueue,&byte_read,portMAX_DELAY)){
            continue;
        }

        /* Process the byte read in */
        snprintf(rpy_str, TEST_BUFFER, "\n\rIMU Reply: %X\n\r",byte_read);
        ROS_Write((uint8_t*)rpy_str, TEST_BUFFER, portMAX_DELAY);

        /* Delay Until Next Loop */
        vTaskDelay(pdMS_TO_TICKS(IMU_TASK_DELAY_MS));    
#endif
#ifdef TEMP_TASK
        /* Request Temp Data from IMU */
        HAL_StatusTypeDef ret = LSM6DS3_Is_Ready(sensor); //!arguments pointer can be changed to a struct pointer
        if(ret) {

            ret = LSM6DS3_ReadTemp(sensor);
            //? Might change the return type for the sensor not sure yet
        }
        /* Send A Temp Data to RTOS Queue */
        if(pdTRUE != xQueueSendToBack(IMU_ReaderQueue, &(sensor->temp_data),portMAX_DELAY)){

            //?Report error
            continue;
        }

        //! Not sure about this for outputting, but just for placeholder
        //snprintf(rpy_str, TEST_BUFFER, "\n\rIMU Reply: %f\n\r",sensor->temp_data);

        vTaskDelay(pdMS_TO_TICKS(IMU_TASK_DELAY_MS));  
        
#endif
    }
}

void IMU_TestTask(void * arguments){
    uint8_t byte = 0;
    while(1)
    {
        /* Generate a random byte value */
        byte = (uint8_t)rand();

        /* Send byte to the IMU Task */
        xQueueSendToBack(IMU_ReaderQueue,&byte,portMAX_DELAY);

        /* Delay Until Next Loop*/
        vTaskDelay(pdMS_TO_TICKS(IMU_TASK_DELAY_MS));
    }
}
/* Public Implementation */

/* Private Implementation */
