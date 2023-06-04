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
QueueHandle_t DebugQueue;

SemaphoreHandle_t DebugMutex;

uint8_t DebugBuf[MAX_USART_BUF_SIZE];
/* Private Prototypes */


void UART_Init(DAT_USART_Handle_t * handle){
    assert(handle);
    // Check if the handle has already been initialized
    if (handle->init == true){

        //Return since we don't need to init anymore?
        return HAL_ERROR;
    }

    else {


        
    // Configure the HAL handle
    
    HAL_UART_Init(handle->uart_h);
    handle->init = true;
    // Configure the RTOS Resources 
    handle->sem_rx = xSemaphoreCreateMutex(); //not sure if we wanted binary or mutex
    handle->sem_tx = xSemaphoreCreateMutex();

    handle->queue_h = xQueueCreate(MAX_USART_QUEUE_SIZE, MAX_USART_BUF_SIZE);
    
    }
    

}

void UART_DeInit(DAT_USART_Handle_t * handle){
    assert(handle);
    
    // Check if the handle has already been initialized
  if (handle->init == true)
    {
        //Since we know that the handle has been initialized we can then go and free up all the resources 

    // De-Init HAL layer
    HAL_UART_DeInit(handle->uart_h);
    // De-allocate RTOS Resources   
    vSemaphoreDelete(handle->sem_rx);
    vSemaphoreDelete(handle->sem_tx);

    vQueueDelete(handle->queue_h);
    handle->init = false;


    }
    else {
        //otherwise we can return with error that the handle has not been initialized yet
        return HAL_ERROR;
    }

}

/* Rx ISR Callback */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    
    BaseType_t xStatus ={0};
    

    if(huart == &huart3){
        xStatus = xQueueSendToBackFromISR(DebugQueue,DebugBuf,NULL);
        Request_Debug_Read();
    }
    if(xStatus == pdPASS){
        
    }

    return;
}

/* Exported Implemtations */
void Request_Debug_Read(void){
    
    HAL_UART_Receive_IT(&huart3,DebugBuf,1);
    return;

}

void EnableDebug(void){
    
    DebugQueue = xQueueCreate(MAX_USART_QUEUE_SIZE, sizeof(uint8_t));

    DebugMutex = xSemaphoreCreateMutex();

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
    xSemaphoreTake(DebugMutex, portMAX_DELAY);
    // Transmit via Debug USART
    HAL_UART_Transmit(&huart3,(uint8_t*)buffer,
        strlen(buffer), HAL_MAX_DELAY);
    // Give the lock 
    xSemaphoreGive(DebugMutex);

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
    HAL_UART_Transmit(&huart3,(uint8_t*)buffer, 
        strlen(buffer), HAL_MAX_DELAY);
    
    return;    
}

void TASKDebugParser(void){
    BaseType_t xStatus = {0};
    uint8_t in;
    uint8_t pos = 0;
    uint8_t buffer[50];
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

/* Private Implemtations */
