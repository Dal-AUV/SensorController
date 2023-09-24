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
#include "i2c.h"
#include "FreeRTOS.h"

#include <stdbool.h>

#include "queue.h"
#include "semphr.h"



/*Private Prototypes*/

void MX_I2C1_Init(void);

/*Exported Functions*/
void I2C_Init(void)
{
	MX_I2C1_Init();
}



//Test reading structure, likely going to discontinue in favour of functions below
HAL_StatusTypeDef IMU_readVal(uint8_t address, uint8_t pinCfg, uint8_t * imudata) {

	HAL_StatusTypeDef ret;
	uint8_t imudata[2];

	ret = HAL_I2C_IsDeviceReady(&hi2c1, (unit16_t)(address<<1), MAX_I2C_TRIAL_COUNT, MAX_I2C_TIMEOUT);
	if(ret != HAL_OK) return ret;

	ret = HAL_I2C_Master_Transmit_IT(&hi2c1, (unit16_t)(address<<1), &pinCfg, 1, MAX_I2C_TIMEOUT);
	if(ret != HAL_OK) return ret;

	for(int i = 0; i<delay_count; i++);

	ret = HAL_I2C_Master_Receive_IT(&hi2c1, (unit16_t)(address<<1) | 0x01, imu_data, 1);
	if(ret != HAL_OK) return ret;

	//Do some data changing to merge/store/something bytes

	return HAL_OK;
}


//Low level HAL Function replacements for use in tasks etc. abstracts some HAL constants out
HAL_StatusTypeDef LSM6DS3_readRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data){
	//Depending on sensor may want to update to I2C_MEMADD_SIZE_16BIT
	return HAL_I2C_Mem_Read_IT(dev->i2c_handle, (LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef LSM6DS3_readRegisters(LSM6DS3 *dev,uint8_t reg, uint8_t * data, uint8_t length){
	//Depending on sensor may want to update to I2C_MEMADD_SIZE_16BIT
	return HAL_I2C_Mem_Read_IT(dev->i2c_handle, (LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef LSM6DS3_writeRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data){
	return	HAL_I2C_Mem_Write_IT(dev->i2c_handle, (LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, 1,HAL_MAX_DELAY);
}

//Init function for sensor, should include sensor ready checks and also configuration settings
HAL_StatusTypeDef LSM6DS3_Init(LSM6DS3 * dev, I2C_HandleTypeDef * i2cHandle) {

	HAL_StatusTypeDef sensor_ret;

	dev->i2c_handle = i2cHandle;


	//Example initializations of data arrays for output
	dev->accel_data[0] = 0.0f;
	dev->accel_data[0] = 0.0f;
	dev->accel_data[0] = 0.0f;

	dev->gryo_data[0] = 0.0f;
	dev->gyro_data[0] = 0.0f;
	dev->gyro_data[0] = 0.0f;

	dev->temp_data = 0.0f;

	//I need to check isDeviceReady's effectiveness, but i've heard it works fine

	sensor_ret = HAL_I2C_IsDeviceReady(dev->i2c_handle, (LSM6DS3_ADDR << 1), MAX_I2C_TRIAL_COUNT, HAL_MAX_DELAY);
	if(sensor_ret != HAL_OK) return sensor_ret;

	// TODO add power control, FIFO, etc configs in init function

}

//Processing function to take data from the IMU and convert it into readable values
//Need to do some bit manip and then conversion math which I will need help with
HAL_StatusTypeDef LSM6DS3_ProcessGyro(LSM6DS3 * dev){

	//Data is 2 bytes signed two's complement

	uint8_t regData[6]; //(x, y, z, each 16 bits
	//XL, XH, YL, YH, ZL, ZH

	HAL_StatusTypeDef status = LSM6DS3_readRegisters(dec, 00100010, regData, 6);

	int16_t gyroRawSigned[3];

	//TODO need to combine the two bits but not sure how the two's complement plays into that
	gyroRawSigned[0] = ((uint16_t) regData[0]);
	gyroRawSigned[1] = 2;
	gyroRawSigned[2] = 3;




	return status;

}

//Interrupt functions that currently are not used

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c1) {

	//Transmission to sensor complete do something !

	if(hi2c1 == &hi2c1) {
		//RTOS Stuff
	}
	return;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c1) {

	//Reception from sensor complete do something !
	//Code copied from USART.C

	BaseType_t xStats = {0};

	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

	    if(hi2c1 == &hi2c1){

	        xStatus = xQueueSendToBackFromISR(ROSReaderQueue,ROSBuf,NULL);

	        //We successfully transmitted to the function now call the receive. (I don't think this is needed with the sensor read function
	        HAL_I2C_Master_Receive_IT(&hi2c1, (unit16_t)(address<<1) | 0x01, imu_data, 1);
	    }
	    if(xStatus == pdPASS){
	        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	    }
	    return;
}

//Default i2c1 port init function

void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20303E5D;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}
