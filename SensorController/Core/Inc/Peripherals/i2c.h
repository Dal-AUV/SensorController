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
#include "main.h"
#include "FreeRTOS.h"

#include <stdbool.h>

#include "queue.h"
#include "semphr.h"
/* Definitions */

#define LSM6DS3_ADDR 1101010b; //7th LSB driven by the SDO/SA0 Pin p.34

//Add Sensor Name Here
typedef enum I2C_SENSORS{
	IMU_LSM6DS3,

	SENSOR_TOTAL
};

typedef struct {
	//handle pointer
	I2C_HandleTypeDef *i2c_handle;

	//data buffers

	uint16_t gyro_data[3]; //x, y ,z
	uint16_t accel_data[3];
	uint16_t temp_data;

}LSM6DS3;

/* Public Functions */
void I2C_Init(void);
//testing example function <-- likely to remove
HAL_StatusTypeDef IMU_readVal(uint8_t address, uint8_t pinCfg, uint8_t * imudata);

//Data processing functions

HAL_StatusTypeDef LSM6DS3_ProcessGyro(LSM6DS3 * dev);

HAL_StatusTypeDef LSM6DS3_Init(LSM6DS3 * dev, I2C_HandleTypeDef * i2cHandle);


//Low level functions

/**
 * @brief Function to handle 1-byte read requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg address of the registering we want to read from
 * @param data pointer to recieve data
 */
HAL_StatusTypeDef LSM6DS3_readRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data);

HAL_StatusTypeDef LSM6DS3_readRegisters(LSM6DS3 *dev,uint8_t reg, uint8_t * data, uint8_t length);
/**
 * @brief Function to handle 1-byte write requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg address of the registering we want to read from
 * @param data pointer to write data from
 */
HAL_StatusTypeDef LSM6DS3_writeRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data);
