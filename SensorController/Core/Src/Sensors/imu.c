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
#define IMU_BLOCK_TIME 10

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
        ret = DAT_ReadRegisters(sensor, WHO_AM_I, buf, 1, I2C_SEMPHR_BLOCK);

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

HAL_StatusTypeDef LSM6DS3_Init(DAT_SENSOR * dev, I2C_HandleTypeDef * i2cHandle) {

	HAL_StatusTypeDef sensor_ret;
	dev->I2C_HANDLE = i2cHandle;
	dev->SENSOR_ADDR =  LSM6DS3_ADDR;
	sensor_ret = HAL_I2C_IsDeviceReady(dev->I2C_HANDLE, (dev->SENSOR_ADDR << 1), 2, HAL_MAX_DELAY);
	if(sensor_ret != HAL_OK) return sensor_ret;

	//Example initializations of data arrays for output
#ifdef generalize
	dev->accel_data[0] = 0.0f;
	dev->accel_data[1] = 0.0f;
	dev->accel_data[2] = 0.0f;

	dev->temp_data = 0.0f;
#endif

	return sensor_ret;
}

/* Processing Functions */

HAL_StatusTypeDef LSM6DS3_Reg_Init(DAT_SENSOR *dev, uint8_t Type) {

	HAL_StatusTypeDef status;

	if(Type == ACCEL_ONLY_ENABLE) {

		uint8_t regConfig = 0x4A;
		/* Configures the Accelerometer control register
		 * Configuration: 0b01001010
		 * Freq: 104Hz < -- Ouput Data Rate Register = 0100
		 * Full-Scale Selction: 4g <-- FS_XL = 10
		 * Bandwith Filter Selection: 100Hz <-- BW_XL = 10
		 */
		//DAT_WriteRegisters(dev, CTRL1_XL, PregConfig);
		status = DAT_WriteRegister(dev, CTRL1_XL, &regConfig, 1, I2C_SEMPHR_BLOCK);
	}
	else if (Type == GYRO_ONLY_ENABLE) {
		//pg. 27
		//write to CTRL2_G
		//write power mode to CTRL7_G
	}
	else {
		//init both
	}

	return status;
}

HAL_StatusTypeDef LSM6DS3_ReadAccel(DAT_SENSOR *dev, float accelData[3]) {
	/* Variable Declarations */
	uint8_t regData[6]; //H & L Bytes for XYZ

	HAL_StatusTypeDef ret;

	int8_t Zsign = 1;
	int8_t Ysign = 1;
	int8_t Xsign = 1;

	uint16_t mergeX = 0;
	uint16_t mergeY = 0;
	uint16_t mergeZ = 0;

	uint16_t mask = 0x8000;


	ret = DAT_ReadRegisters(dev, OUTX_L_XL, regData, 6, I2C_SEMPHR_BLOCK); // Registers are all adjacent RegData[0] = XL, RegData[6] = ZH;

	mergeX |= ((uint16_t)regData[0] | (uint16_t)regData[1]<<8);
	mergeY |= ((uint16_t)regData[2] | (uint16_t)regData[3]<<8);
	mergeZ |= ((uint16_t)regData[4] | (uint16_t)regData[5]<<8);

	/* Two's complement conversions for X, Y, Z */
	if((mergeY & mask) == 0) {
		Ysign = 1;
	}
	else {
		mergeY = (~mergeY) + 1;
		Ysign = -1;
	}
	if((mergeX & mask) == 0) {
		Xsign = 1;
	}
	else {
		mergeX = (~mergeX) + 1;
		Xsign = -1;
	}
	if((mergeZ & mask) == 0) {
		Zsign = 1;
	}
	else {
		mergeZ = (~mergeZ) + 1;
		Zsign = -1;
	}

	/*float zOut = (float)sign * mergeZ * 4.0/32768.0 <-- Alternate Conversion */
	/* At +-2g conversion = 0.061, since accel is set to +-4g double to 0.122 */
	float xOut_Mg = (float)Xsign * mergeX * 0.122;
	float yOut_Mg = (float)Ysign * mergeY * 0.122;
	float zOut_Mg = (float)Zsign * mergeZ * 0.122;

	//float zOut_MS = zOut_Mg / 101.97162129779 <-- alternate conversion to m/s^2;
	float xOut_MS = xOut_Mg * 0.00980665;
	float yOut_MS = yOut_Mg * 0.00980665;
	float zOut_MS = zOut_Mg * 0.00980665;

	/* Set struct data to calculated values */
#ifdef generalize
	dev->accel_data[0] = xOut_MS;
	dev->accel_data[1] = yOut_MS;
	dev->accel_data[2] = zOut_MS;
#endif
	accelData[0] = xOut_MS;
	accelData[1] = yOut_MS;
	accelData[2] = zOut_MS;

	return ret;
}

/* Private Implementation */
