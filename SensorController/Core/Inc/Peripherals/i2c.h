/**
 * @file i2c.h
 * @author Wyatt Shaw
 * @brief Header file for the I2C hardware interfaces
 * @version 0.1
 * @date 2023-09-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "main.h"
#include "FreeRTOS.h"
#include <string.h>

#include <stdbool.h>

#include "queue.h"
#include "semphr.h"
/* Definitions */
extern I2C_HandleTypeDef hi2c1;
//const uint16_t LSM6DS3_ADDR = 1101010b; //7th LSB driven by the SDO/SA0 Pin p.34

#define LOCKED_ADDR 0b1101011 //7th bit is set to SA0 Pin (GND), so we could use two addresses and dynamically change which one

#define DELAY_COUNT 5000
#define MAX_I2C_TRIAL_COUNT 10
#define ACCEL_ONLY_ENABLE 0
#define GYRO_ONLY_ENABLE 1
#define BOTH_ENABLE 2

/*-------------------------- Register Defines ----------------------- */

#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define CTRL4_C 0x13
#define CTRL5_C 0x14
#define CTRL6_C 0x15
#define CTRL7_G 0x16
#define CTRL8_XL 0x17
#define CTRL9_XL 0x18
#define CTRL10_C 0x19
#define OUT_TEMP_L 0x20
#define OUT_TEMP_H 0x21
#define OUTX_L_G 0x22
#define OUTX_H_G 0x23
#define OUTY_L_G 0x24
#define OUTY_H_G 0x25
#define OUTZ_L_G 0x26
#define OUTZ_H_G 0x27
#define OUTX_L_XL 0x28
#define OUTX_H_XL 0x29
#define OUTY_L_XL 0x2A
#define OUTY_H_XL 0x2B
#define OUTZ_L_XL 0x2C
#define OUTZ_H_XL 0x2D

/* Green Wire = SDA, Yellow Wire = SCL */

//Add Sensor Name Here
typedef enum I2C_SENSORS{
	IMU_LSM6DS3,

	I2C_SENSOR_TOTAL
}ACTIVE_SENSORS;

typedef struct {
	//handle pointer
	I2C_HandleTypeDef *i2c_handle;
	uint16_t LSM6DS3_ADDR;//7th LSB driven by the SDO/SA0 Pin p.34

	//data buffers

	uint16_t gyro_data[3]; //x, y ,z
	uint16_t accel_data[3];
	uint16_t temp_data;

}LSM6DS3;

/* Public Functions */
void I2C_Init(void);

//Data processing functions

HAL_StatusTypeDef LSM6DS3_ReadTemp(LSM6DS3 * dev);

HAL_StatusTypeDef LSM6DS3_Init(LSM6DS3 * dev, I2C_HandleTypeDef * i2cHandle);

//Low level functions

/**
 * @brief Function to handle n-byte read requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg address of the register we want to read from
 * @param data pointer to recieve data
 * @param number of bytes to be read
 * @return HAL_OK if no errors
 */
HAL_StatusTypeDef LSM6DS3_readRegisters(LSM6DS3 *dev,uint8_t reg, uint8_t * data, uint8_t length);
/**
 * @brief Function to handle 1-byte write requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg internal address of register we weant to we want to read from
 * @param data pointer to write data from
 */
HAL_StatusTypeDef LSM6DS3_writeRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data);

/**
 * @brief check to see if the sensor will return an ACK signal
 */
HAL_StatusTypeDef LSM6DS3_Is_Ready(LSM6DS3 *dev);

/**
 * @brief Initiate Registers to configure collection of Acceleration and Gyro Data
 * @param pointer to sensor struct
 */
HAL_StatusTypeDef LSM6DS3_Reg_Init(LSM6DS3 *dev, uint8_t Type);

HAL_StatusTypeDef LSM6DS3_ReadAccel(LSM6DS3 *dev);
