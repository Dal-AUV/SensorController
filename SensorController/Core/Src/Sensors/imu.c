/**
 * @file imu.c
 * @author Matthew Cockburn and Wyatt Shaw
 * @brief IMU Sensor C Source File
 * @version 1.1
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
#define TEST_BUFFER 75

/* Data Structures */

/* Global Variables */

/* Private Prototypes */

/* RTOS Tasks */
void IMU_Task(DAT_SENSOR * sensor){

    char rpy_str[TEST_BUFFER];
    LSM6DS3_Init(sensor,&hi2c1);
    memset(rpy_str,0,TEST_BUFFER);

    /* Data Buffers */
	float gyro_data[3]; //x, y ,z
	float accel_data[3];
	float temp_data;

    while(1)
    {
        /* Temp Buffer for WHO AM I check */
        uint8_t buf[1];

        HAL_StatusTypeDef ret;

        ret = LSM6DS3_Reg_Init(sensor, ACCEL_ONLY_ENABLE);
        ret = DAT_ReadRegisters(sensor, WHO_AM_I, buf, 1);

        //If we can read from who am I then we can try to read from the rest of the sensors
        if(ret == HAL_OK) {
        	LSM6DS3_ReadAccel(sensor, accel_data);
        	snprintf(rpy_str, TEST_BUFFER, "\n\rX Data: %2.3fm/s^2\n\r\n\rY Data: %2.3fm/s^2\n\r\n\rZ Data: %2.3fm/s^2\n\r",accel_data[0],accel_data[1],accel_data[2]);
        	ROS_Write(rpy_str, TEST_BUFFER, portMAX_DELAY);

        }
        vTaskDelay(pdMS_TO_TICKS(IMU_TASK_DELAY_MS));
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
