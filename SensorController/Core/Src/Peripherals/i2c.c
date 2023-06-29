/**
 * @file i2c.c
 * @author Wyatt Shaw
 * @brief Source file for the I2C hardware interfaces
 * @version 0.1
 * @date 2023-06-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */


/* Standard Headers*/
#include "main.h"
#include <stdarg.h>
#include <assert.h>
#include <string.h>
/* Middleware Headers */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
/* Application Headers */
#include "Peripherals/i2c.h"


/* Definitions */
I2C_HandleTypeDef hi2c1;

/* Functions */
DAT_I2C_HANDLE_t I2Cs[I2C_Totals] = {
	  //{UART_HANLDE, QUEUE, SEMRX, SEMTX, INIT, SENSOR_NAME}
};

HAL_StatusTypeDef I2C_Init(struct I2C_Interface * handle) {
    assert(handle);

    /* check if handle has been init*/
    if (handle->initFlag) return HAL_OK;    

    /* configure i2c handle*/
    hi2c1.Instance = handle->MEM_BASE;
    hi2c1.Init.Timing = 0x20303E5D;
    hi2c1.Init.OwnAddress1 = handle->OWN_ADDRESS;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    
    /* configure the RTOS resources*/
    if(NULL == (handle->sem_rx = xSemaphoreCreateMutex())) return HAL_ERROR;
    if(NULL == (handle->sem_tx = xSemaphoreCreateMutex())) return HAL_ERROR;
    if(NULL == (handle->queue_h = xQueueCreate(MAX_USART_QUEUE_SIZE,
    		MAX_USART_BUF_SIZE))) return HAL_ERROR;

    return HAL_I2C_Init(handle->i2c_h);
}

void I2C_DeInit(DAT_I2C_HANDLE_t *handle) {
    assert(handle);

    if (handle->initFlag == true)
    {
        HAL_I2C_DeInit(handle->i2c_h);
        vSemaphoreDelete(handle->sem_rx);
        vSemaphoreDelete(handle->sem_tx);
        vQueueDelete(handle->queue_h);
        handle->initFlag = false;
    }
    else return HAL_OK;
}