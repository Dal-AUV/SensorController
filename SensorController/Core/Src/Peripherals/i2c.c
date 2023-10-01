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
#include "Peripherals/i2c.h"
#include "FreeRTOS.h"

#include <stdbool.h>

#include "queue.h"
#include "semphr.h"

#define ARDUINO_COPY


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

	ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(address<<1), MAX_I2C_TRIAL_COUNT, MAX_I2C_TIMEOUT);
	if(ret != HAL_OK) return ret;

	ret = HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)(address<<1), &pinCfg, 1, MAX_I2C_TIMEOUT);
	if(ret != HAL_OK) return ret;

	for(int i = 0; i<delay_count; i++);

	ret = HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)(address<<1) | 0x01, imu_data, 1);
	if(ret != HAL_OK) return ret;

	//Do some data changing to merge/store/something bytes

	return HAL_OK;
}

HAL_StatusTypeDef LSM6DS3_Is_Ready(LSM6DS3 *dev) {

	return HAL_I2C_IsDeviceReady(dev->i2c_handle, dev->LSM6DS3_ADDR, HAL_MAX_TRIAL, HAL_MAX_DELAY);
}

//Low level HAL Function replacements for use in tasks etc. abstracts some HAL constants out
/**
 * @brief Function to handle read requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg address of the register we want to read from
 * @param data pointer to recieve data
 * @param length 
 */
HAL_StatusTypeDef LSM6DS3_readRegisters(LSM6DS3 *dev,uint8_t reg, uint8_t * data, uint8_t length){
	//Depending on sensor may want to update to I2C_MEMADD_SIZE_16BIT
	
	return HAL_I2C_Mem_Read_IT(dev->i2c_handle, (dev->LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length);
}

/**
 * @brief Function to handle 1-byte write requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg internal address of register we weant to we want to read from
 * @param data pointer to write data from
 */
HAL_StatusTypeDef LSM6DS3_writeRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data){
	return	HAL_I2C_Mem_Write_IT(dev->i2c_handle, (dev->LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, 1);
}

//Init function for sensor, should include sensor ready checks and also configuration settings
HAL_StatusTypeDef LSM6DS3_Init(LSM6DS3 * dev, I2C_HandleTypeDef * i2cHandle) {

	HAL_StatusTypeDef sensor_ret;

	dev->i2c_handle = i2cHandle;


	//Example initializations of data arrays for output
	dev->accel_data[0] = 0.0f;
	dev->accel_data[1] = 0.0f;
	dev->accel_data[2] = 0.0f;
	
	dev->temp_data = 0.0f;

	//I need to check isDeviceReady's effectiveness, but i've heard it works fine

	sensor_ret = HAL_I2C_IsDeviceReady(dev->i2c_handle, (dev->LSM6DS3_ADDR << 1), MAX_I2C_TRIAL_COUNT, HAL_MAX_DELAY);
	if(sensor_ret != HAL_OK) return sensor_ret;

	// TODO add power control, FIFO, etc configs in init function

	//CTRL4_C Register to enable temp as 4th fifo data set?

}

//Processing function to take data from the IMU and convert it into readable values
//Need to do some bit manip and then conversion math which I will need help with
HAL_StatusTypeDef LSM6DS3_ReadTemp(LSM6DS3 * dev){

	//Data is 2 bytes signed two's complement

	uint8_t regData[2]; //Temp data comes in two bytes

	HAL_StatusTypeDef status = LSM6DS3_readRegisters(dev, LSM6DS3_TEMPL_ADDR, regData, 2);

	if (status != HAL_OK)
	{
		return status;
	}
	

	int16_t signed_twobyte_temp = 0x0000;


#ifdef TWOS_TEST
	//reg[0] is tempL reg[1] is tempH
	signed_twobyte_temp |= (((int16_t)regData[1] << 8) | (int16_t)regData[0]);
	
	//two's complement conversion to decimal

	uint16_t sign_mask = 0x8000;

	if (signed_twobyte_temp & sign_mask != 0) //check if the msb is 1
	{
		signed_twobyte_temp = -(~(signed_twobyte_temp)+1); //flip all the bits and add 1
	}
	
	//!unsure if this conversion wil work okay
	//!I feel like there should be something in the documentation for converting the bin --> real value but I haven't seen anythign
	//!From a different sensor of theirs 1 bit = 0.0625 C
	//https://github.com/sparkfun/SparkFun_TMP102_Arduino_Library/blob/master/src/SparkFunTMP102.cpp

	dev->temp_data = signed_twobyte_temp*0.0625;
#endif
#ifdef ARDUINO_COPY

	//source
	//https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library/blob/master/src/SparkFunLSM6DS3.cpp#L747

	signed_twobyte_temp |= (((int16_t)regData[1] << 8) | (int16_t)regData[0]);

	float temp_output = (float) signed_twobyte_temp /16; // divide 16 to scale
	temp_output += 25; // add to remove offset
	dev->temp_data = temp_output;
#endif
	return status;

}

//Interrupt functions that currently are not used

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
