/**
 * @file usart.c
 * @author Matthew Cockburn & Wyatt Davion
 * @brief Source file for the USART hardware interfaces, contains ISR 
 * implemetation, Recieve and Transmit functions
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/* Standard Headers */
#include "main.h"
#include <stdarg.h>
#include <assert.h>
#include <string.h>
/* Middleware Headers */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
/* Application Headers */
#include "Peripherals/usart.h"

/* Macros */

/* Definitions */
#define USART_TX_BUF_SIZE 50
/* Public Variables */


// HAL Hardware Handles
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart2;
// UART Queues
QueueHandle_t DebugQueue;
// UART Interface Locks
SemaphoreHandle_t DebugMutex;
// Receive Buffers
uint8_t ROSBuf[MAX_USART_BUF_SIZE];

// Global Definitions of DAT UART API Handles
/**
 * @brief 
 * 
 */
DAT_USART_Handle_t uarts[SENSOR_TOTAL] ={
    {
        .uart_h = &huart3,
        .h_addr = USART3,
        .queue_h = NULL,
        .buf = ROSBuf,
        .sem_tx = NULL,
        .init = UART_Init,
        .deinit = UART_DeInit,
        .enable = UART_Enable,
        .disable = UART_Disable,
        .write = UART_Write,
        .write_unpro = UART_Write_Unprotected
    }
};

/* Rx ISR Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    
    BaseType_t xStatus ={0};
    
    //sanity check pin toggle to see if Callback is running will flash red light
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); 

    if(huart == uarts[ROS].uart_h){
        
        xStatus = xQueueSendToBackFromISR(uarts[ROS].queue_h,uarts[ROS].buf,NULL);
        uarts[ROS].enable(&uarts[ROS]);
    }
    if(xStatus == pdPASS){
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); 
    }
    return;
}

/* Public Functions */

HAL_StatusTypeDef Sys_UART_Init(void){

	for (int i = 0 ; i < SENSOR_TOTAL; ++i){
        if (uarts[i].uart_h == NULL) return HAL_ERROR;
		uarts[i].init(&uarts[i]);
		uarts[i].enable(&uarts[i]);
	}
	return HAL_OK;
}

HAL_StatusTypeDef UART_Init(struct UART_Interface * handle){

	assert(handle);
    
    // Check if the handle has already been initialized

    if (handle->initFlag) return HAL_OK;

    // Configure the HAL handle
    handle->uart_h->Instance = handle->h_addr;
    handle->uart_h->Init.BaudRate = 115200;
    handle->uart_h->Init.WordLength = UART_WORDLENGTH_8B;
    handle->uart_h->Init.StopBits = UART_STOPBITS_1;
    handle->uart_h->Init.Parity = UART_PARITY_NONE;
    handle->uart_h->Init.Mode = UART_MODE_TX_RX;
    handle->uart_h->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    handle->uart_h->Init.OverSampling = UART_OVERSAMPLING_16;
    handle->uart_h->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    handle->uart_h->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    handle->initFlag = true;

    // Configure the RTOS Resources 
    if(NULL == (handle->sem_rx = xSemaphoreCreateMutex())) return HAL_ERROR;
    if(NULL == (handle->sem_tx = xSemaphoreCreateMutex())) return HAL_ERROR;
    if(NULL == (handle->queue_h = xQueueCreate(MAX_USART_QUEUE_SIZE,
    		MAX_USART_BUF_SIZE))) return HAL_ERROR;

    return HAL_UART_Init(handle->uart_h);
}

HAL_StatusTypeDef UART_DeInit(struct UART_Interface * handle){
    assert(handle);
    
    // Check if the handle has already been initialized
    if (!handle->initFlag) return HAL_OK;
    // De-Init HAL layer
    //TODO we should handle this return case
    HAL_StatusTypeDef status = HAL_UART_DeInit(handle->uart_h);
    
    // De-allocate RTOS Resources   
    vSemaphoreDelete(handle->sem_rx);
    vSemaphoreDelete(handle->sem_tx);

    vQueueDelete(handle->queue_h);
    handle->initFlag = false;

    return status;
}

HAL_StatusTypeDef UART_Enable(struct UART_Interface * handle){
    assert(handle);

    return HAL_UART_Receive_IT(handle->uart_h,handle->buf,UART_RX_ISR_TRIGGER_SZ);
}

HAL_StatusTypeDef UART_Disable(struct UART_Interface * handle){
    assert(handle);

    return HAL_UART_AbortReceive_IT(handle->uart_h);
}

HAL_StatusTypeDef UART_Write(struct UART_Interface * handle,
    const uint8_t* buf,const uint16_t size){
    
    assert(handle);
    assert(buf);

    if(size == 0) return HAL_OK;
    // Take the Tx lock
    xSemaphoreTake(handle->sem_tx,portMAX_DELAY);
    HAL_StatusTypeDef status = HAL_UART_Transmit(handle->uart_h,
        buf,size,portMAX_DELAY);
    // Release the Tx Lock
    xSemaphoreGive(handle->sem_tx);
    return status;
}

HAL_StatusTypeDef UART_Write_Unprotected(struct UART_Interface * h,
    const uint8_t* buf,const uint16_t size){
    
    assert(h);
    assert(buf);

    if(size == 0) return HAL_OK;

    return HAL_UART_Transmit(h->uart_h,buf,size,portMAX_DELAY);
}
