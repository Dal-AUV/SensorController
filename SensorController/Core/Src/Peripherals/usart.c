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


void UART_Init(UART_Interface * handle){
    assert(handle);
    // Check if the handle has already been initialized

    // Configure the HAL handle

    // Configure the RTOS Resources
    

}

void UART_DeInit(UART_Interface * handle){
    assert(handle);
    
    // Check if the handle has already been initialized

    // De-Init HAL layer

    // De-allocate RTOS Resources

}




/* Rx ISR Callback */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    
    BaseType_t xStatus ={0};
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

    if(huart == &huart3){
        xStatus = xQueueSendToBackFromISR(DebugQueue,DebugBuf,NULL);
        Request_Debug_Read();
    }
    if(xStatus == pdPASS){
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
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
