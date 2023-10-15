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

#define TEST_CONVERSION


/*Private Prototypes*/

void MX_I2C1_Init(void);

/*Exported Functions*/
void I2C_Init(void)
{
	MX_I2C1_Init();

}

I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef LSM6DS3_Is_Ready(LSM6DS3 *dev) {

	return HAL_I2C_IsDeviceReady(dev->i2c_handle, (dev->LSM6DS3_ADDR << 1), 10, HAL_MAX_DELAY);
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
	return HAL_I2C_Mem_Read(dev->i2c_handle,(dev->LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

/**
 * @brief Function to handle 1-byte write requests to a specific register on board the IMU
 * @param dev struct for handle
 * @param reg internal address of register we weant to we want to read from
 * @param data pointer to write data from
 */
HAL_StatusTypeDef LSM6DS3_writeRegister(LSM6DS3 *dev,uint8_t reg, uint8_t * data){
	return	HAL_I2C_Mem_Write(dev->i2c_handle, (dev->LSM6DS3_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, 1,HAL_MAX_DELAY);
}

//Init function for sensor, should include sensor ready checks and also configuration settings
HAL_StatusTypeDef LSM6DS3_Init(LSM6DS3 * dev, I2C_HandleTypeDef * i2cHandle) {

	HAL_StatusTypeDef sensor_ret;
	dev->i2c_handle = i2cHandle;
	dev->LSM6DS3_ADDR =  LOCKED_ADDR;
	sensor_ret = HAL_I2C_IsDeviceReady(dev->i2c_handle, (dev->LSM6DS3_ADDR << 1), 2, HAL_MAX_DELAY);
	if(sensor_ret != HAL_OK) return sensor_ret;



	//Example initializations of data arrays for output
	dev->accel_data[0] = 0.0f;
	dev->accel_data[1] = 0.0f;
	dev->accel_data[2] = 0.0f;
	
	dev->temp_data = 0.0f;

	//(void)LSM6DS3_Reg_Init(dev);

	return sensor_ret;
}

//Processing function to take data from the IMU and convert it into readable values
//Need to do some bit manip and then conversion math which I will need help with
HAL_StatusTypeDef LSM6DS3_ReadTemp(LSM6DS3 * dev){

	//Data is 2 bytes signed two's complement

	uint8_t  regData[2]; //Temp data comes in two bytes

	HAL_StatusTypeDef status = LSM6DS3_readRegisters(dev, OUT_TEMP_L, regData, 2);

	if (status != HAL_OK)
	{
		return status;
	}
	
	int16_t signed_twobyte_temp = 0x0000;

#ifdef TEST_CONVERSION
	//source
	//https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library/blob/master/src/SparkFunLSM6DS3.cpp#L747

	signed_twobyte_temp |= ((int16_t)regData[0] |(int16_t)regData[1]<<8);

	float temp_output = (float) signed_twobyte_temp /16; // divide 16 to scale
	temp_output += 25; // add to remove offset
	dev->temp_data = temp_output;
#endif
	return status;

}

HAL_StatusTypeDef LSM6DS3_Reg_Init(LSM6DS3 *dev, uint8_t Type) {

	HAL_StatusTypeDef status;

	if(Type == ACCEL_ONLY_ENABLE) {
		uint8_t *PregConfig;
		uint8_t regConfig = 0x4A;
		PregConfig = &regConfig;
		/* Configures the Accelerometer control register
		 * Configuration: 0b01001010
		 * Freq: 104Hz < -- Ouput Data Rate Register = 0100
		 * Full-Scale Selction: 4g <-- FS_XL = 10
		 * Bandwith Filter Selection: 100Hz <-- BW_XL = 10
		 */
		LSM6DS3_writeRegister(dev, CTRL1_XL, PregConfig);
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

HAL_StatusTypeDef LSM6DS3_ReadAccel(LSM6DS3 *dev) {
	uint8_t regData[6]; //H & L Bytes for XYZ

	HAL_StatusTypeDef ret;

	int8_t Zsign = 1;
	int8_t Ysign = 1;
	int8_t Xsign = 1;

	ret = LSM6DS3_readRegisters(dev, OUTX_L_XL, regData, 6); // Registers are all adjacent RegData[0] = XL, RegData[6] = ZH;
	uint16_t mergeX = 0;
	uint16_t mergeY = 0;
	uint16_t mergeZ = 0;
	mergeX |= ((uint16_t)regData[0] | (uint16_t)regData[1]<<8);
	mergeY |= ((uint16_t)regData[2] | (uint16_t)regData[3]<<8);
	mergeZ |= ((uint16_t)regData[4] | (uint16_t)regData[5]<<8);

	uint16_t mask = 0x8000;

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

	//float zOut = (float)sign * mergeZ * 4.0/32768.0;
	/* At +-2g conversion = 0.061, since accel is set to +-4g double to 0.122 */
	float xOut_Mg = (float)1 * mergeX * 0.122;
	float yOut_Mg = (float)Ysign * mergeY * 0.122;
	float zOut_Mg = (float)Zsign * mergeZ * 0.122;

	//float zOut_MS = zOut_Mg / 101.97162129779 <-- alternate conversion to m/s^2;
	float xOut_MS = xOut_Mg * 0.00980665;
	float yOut_MS = yOut_Mg * 0.00980665;
	float zOut_MS = zOut_Mg * 0.00980665;


	return ret;
}


/*-----------------Interrupt functions that currently are not used-------------*/

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c1) {

	//Reception from sensor complete do something !
	//Code copied from USART.C

	BaseType_t xStatus = {0};

	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

	    if(hi2c1 == &hi2c1){

	       //xStatus = xQueueSendToBackFromISR(ROSReaderQueue,ROSBuf,NULL);

	        //We successfully transmitted to the function now call the receive. (I don't think this is needed with the sensor read function
	        //HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t)(address<<1) | 0x01, imu_data, 1);
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
