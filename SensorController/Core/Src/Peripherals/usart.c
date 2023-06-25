/**
 * @file usart.c
 * @author Matthew Cockburn
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
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart2;
QueueHandle_t DebugQueue;
DAT_USART_Handle_t uart3;
SemaphoreHandle_t DebugMutex;
uint8_t DebugBuf[MAX_USART_BUF_SIZE];


/* Private Prototypes */

DAT_USART_Handle_t uarts[SENSOR_TOTAL] = {
	  //{UART_HANLDE, QUEUE, SEMRX, SEMTX, INIT, SENSOR_NAME}
		{&huart3, NULL, NULL, NULL, false, SENSOR1},
		{&huart2, NULL, NULL, NULL, false, SENSOR2},

};


/* Public Functions */

HAL_StatusTypeDef Sys_UART_Init(void){


	for (int i = 0 ; i < SENSOR_TOTAL; ++i){
        
        if (uarts[i].uart_h == 0x0) return HAL_ERROR;
        
		UART_Init(&uarts[i]);

	}

	//return HAL_OK;
	return UART_Init(&uart3);
}

HAL_StatusTypeDef UART_Init(DAT_USART_Handle_t * handle){

	assert(handle);
    
    // Check if the handle has already been initialized

    if (handle->init) return HAL_OK;

    // Configure the HAL handle
    handle->uart_h->Instance = USART3;
    handle->uart_h->Init.BaudRate = 115200;
    handle->uart_h->Init.WordLength = UART_WORDLENGTH_8B;
    handle->uart_h->Init.StopBits = UART_STOPBITS_1;
    handle->uart_h->Init.Parity = UART_PARITY_NONE;
    handle->uart_h->Init.Mode = UART_MODE_TX_RX;
    handle->uart_h->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    handle->uart_h->Init.OverSampling = UART_OVERSAMPLING_16;
    handle->uart_h->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    handle->uart_h->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


    handle->init = true;

    // Configure the RTOS Resources 
    if(NULL == (handle->sem_rx = xSemaphoreCreateMutex())) return HAL_ERROR;
    if(NULL == (handle->sem_tx = xSemaphoreCreateMutex())) return HAL_ERROR;
    if(NULL == (handle->queue_h = xQueueCreate(MAX_USART_QUEUE_SIZE,
    		MAX_USART_BUF_SIZE))) return HAL_ERROR;
    
    return HAL_UART_Init(&handle->uart_h);
}

void UART_DeInit(DAT_USART_Handle_t * handle){
    assert(handle);
    
    // Check if the handle has already been initialized
  if (handle->init == true)
    {
    // De-Init HAL layer
    HAL_UART_DeInit(&(handle->uart_h));
    
    // De-allocate RTOS Resources   
    vSemaphoreDelete(handle->sem_rx);
    vSemaphoreDelete(handle->sem_tx);

    vQueueDelete(handle->queue_h);
    handle->init = false;

    }
    else {
        //Already DeInit so we can just HAL_OK
        return HAL_OK;
    }
}

/* Rx ISR Callback */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    
    BaseType_t xStatus ={0};
    
    //sanity check pin toggle to see if Callback is running will flash red light
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); 

    if(huart == &huart3){
        
        xStatus = xQueueSendToBackFromISR(uart3.queue_h,DebugBuf,NULL); 

        Request_Debug_Read();
    }
    if(xStatus == pdPASS){
        
    }
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); 
    return;
}

/* Exported Implementations */
void Request_Debug_Read(void){
    
    HAL_UART_Receive_IT(&huart3,DebugBuf,1);
    return;

}

void EnableDebug(void){
    
	//UART_Init(&uart3);
    Request_Debug_Read();

    return;

}

void DebugWrite(const char * format, ...){
    
    char buffer[50];
    va_list args;
    // Get Format and parse it
    va_start(args, format);
    vsnprintf(buffer,sizeof(buffer),format, args);
    va_end(args);
    // Take the Debug Lock
    xSemaphoreTake(uart3.sem_tx, portMAX_DELAY);
    // Alternate Transmit via DAT USART
     HAL_UART_Transmit(&uart3.uart_h,(uint8_t*)buffer,
        strlen(buffer), HAL_MAX_DELAY);
    // Give the lock 

    xSemaphoreGive(uart3.sem_tx);

    return;
}

void DebugWriteUnprotected(const char * format, ...){
    
    char buffer[100];
    va_list args;
    // Get Format and parse it
    va_start(args, format);
    vsnprintf(buffer,sizeof(buffer),format, args);
    va_end(args);
    // Transmit via Debug Lock
    HAL_UART_Transmit(&uart3,(uint8_t*)buffer,
        strlen(buffer), HAL_MAX_DELAY);
    
    return;    
}

void TASKDebugParser(void){
    BaseType_t xStatus = {0};
    uint8_t in;
    uint8_t pos = 0;
    uint8_t buffer[50];

    EnableDebug();
    while(1){
        xStatus = xQueueReceive(DebugQueue,&in,portMAX_DELAY);
        if(xStatus == pdFALSE) continue;
        buffer[pos++] = in;
        if((in == '\n') | (in == '\r')){
            pos = 0;
            DebugWrite("%s\n",buffer);
            memset(&buffer,0,50);
        }else continue;
    }
    return;
}
