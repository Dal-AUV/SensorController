/**
 * @file imu.h
 * @author Matthew Cockburn
 * @brief 
 * @version 1
 * @date 2023-09-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _INC_IMU_H_
#define _INC_IMU_H_

/* Headers */
#include "Peripherals/i2c.h"
/* Macros */
#define LSM6DS3_ADDR 0b1101011 //7th bit is set to SA0 Pin (GND), so we could use two addresses and dynamically change which one
/* Data Structures */

/* Global Variables */

/* Public Prototypes */
/* ----- Data processing functions ----- */

/**
 * @brief Reads acceleration registers and stores register data in struct
 * @param dev pointer to sensor struct
 * @param accelData float buffer to hold acceleration data output
 */
HAL_StatusTypeDef LSM6DS3_ReadAccel(DAT_SENSOR *dev, float accelData[3]);
/**
 * @brief Reads the temperature registers and stores in struct
 * @param dev pointer to sensor struct
 */
HAL_StatusTypeDef LSM6DS3_ReadTemp(DAT_SENSOR * dev);

HAL_StatusTypeDef LSM6DS3_Init(DAT_SENSOR * dev, I2C_HandleTypeDef * i2cHandle);

/**
 * @brief IMU Sensor Test Task
 * 
 * @param arguments none
 */
void IMU_Task(DAT_SENSOR * sensor);
/**
 * @brief Test Task for the IMU Sensor
 * 
 * @param arguments none
 */
void IMU_TestTask(void * arguments);
#endif /* _INC_IMU_H_ */
