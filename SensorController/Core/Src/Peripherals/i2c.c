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
#include "System/OS_Ctrl.h"
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

HAL_StatusTypeDef DAT_ReadRegisters(DAT_SENSOR *dev,uint8_t reg, uint8_t * data, uint8_t length, uint32_t blockTime){
	HAL_StatusTypeDef ret;
	xSemaphoreTake(I2CCommandSemphr, pdMS_TO_TICKS(blockTime));
	ret = HAL_I2C_Mem_Read(dev->I2C_HANDLE,(dev->SENSOR_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
	xSemaphoreGive(I2CCommandSemphr);
	return ret;
}

HAL_StatusTypeDef DAT_WriteRegister(DAT_SENSOR *dev,uint8_t reg, uint8_t * data, uint8_t length, uint32_t blockTime){
	HAL_StatusTypeDef ret;
	xSemaphoreTake(I2CCommandSemphr, pdMS_TO_TICKS(blockTime));
	ret = HAL_I2C_Mem_Write(dev->I2C_HANDLE, (dev->SENSOR_ADDR << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
	xSemaphoreGive(I2CCommandSemphr);
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
