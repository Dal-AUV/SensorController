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

/* Data Structures */

/* Global Variables */

/* Public Prototypes */
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
