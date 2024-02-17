/**
 * @file i2c.h
 * @author Wyatt Shaw
 * @brief Header file for the I2C hardware interfaces
 * @version 1.1
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

HAL_StatusTypeDef DAT_SensorIsReady(DAT_SENSOR *dev) {

	return HAL_I2C_IsDeviceReady(dev->I2C_HANDLE, (dev->SENSOR_ADDR << 1), 10, HAL_MAX_DELAY);
}

/* Low level HAL Function replacements for use in tasks etc. abstracts some HAL constants out */
// TODO just use sempaphore stuff lol

HAL_StatusTypeDef DAT_ReadRegisters(DAT_SENSOR *dev,uint8_t reg, uint8_t * data, uint8_t length){
	return HAL_I2C_Mem_Read(dev->I2C_HANDLE,(dev->SENSOR_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

HAL_StatusTypeDef DAT_WriteRegister(DAT_SENSOR *dev,uint8_t reg, uint8_t * data, uint8_t length){
	return	HAL_I2C_Mem_Write(dev->I2C_HANDLE, (dev->SENSOR_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

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
		DAT_WriteRegister(dev, CTRL1_XL, &regConfig, 1);
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


	ret = DAT_ReadRegisters(dev, OUTX_L_XL, regData, 6); // Registers are all adjacent RegData[0] = XL, RegData[6] = ZH;

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


/*-----------------Interrupt functions that currently are not used-------------*/

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c1) {
	return;
}

void HAL_I2C_MasterRxCpltCallback (I2C_HandleTypeDef * hi2c)
{
	return;
  // RX Done .. Do Something!
}
/*Default i2c1 port init function */

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
