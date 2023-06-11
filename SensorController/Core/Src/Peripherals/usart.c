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
UART_HandleTypeDef huart3;
/* Public Variables */
QueueHandle_t DebugQueue;
DAT_USART_Handle_t *uart3;


SemaphoreHandle_t DebugMutex;

uint8_t DebugBuf[MAX_USART_BUF_SIZE];
/* Private Prototypes */


void UART_Init(DAT_USART_Handle_t * handle){
    assert(handle);
    handle->uart_h = &huart3;
    
    // Check if the handle has already been initialized
    if (handle->init){

        //Already Init so we can just HAL_OK
        
        return HAL_OK;
    }

    else {

    // Configure the HAL handle
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
    //not updating in the stm debugger
    handle->init = true;
    handle->status = UART_STATUS_WRITE;
    // Configure the RTOS Resources 
    handle->sem_rx = xSemaphoreCreateMutex();
    handle->sem_tx = xSemaphoreCreateMutex();

    handle->queue_h = xQueueCreate(MAX_USART_QUEUE_SIZE, MAX_USART_BUF_SIZE);
    
    /* Configuration of the RTOS resources does seem to be successfully occuring. However 
    Attempting to change the other values of the handle members is not, uncertain of the cause..*/
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
        //This line doesn't work, the program freezes
        xStatus = xQueueSendToBackFromISR(uart3->queue_h,DebugBuf,NULL); //pointer to pointer issue maybe

        //this is the before
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
    
	//DAT_USART_Handle_t uart3;
    //DebugQueue = xQueueCreate(MAX_USART_QUEUE_SIZE, sizeof(uint8_t));

    //DebugMutex = xSemaphoreCreateMutex();
	UART_Init(&uart3);
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

    //Transmit via Debug USART
    HAL_UART_Transmit(&huart3,(uint8_t*)buffer,
        strlen(buffer), HAL_MAX_DELAY);

    // Alternate Transmit via DAT USART
    // HAL_UART_Transmit(&uart3,(uint8_t*)buffer,
    //    strlen(buffer), HAL_MAX_DELAY);

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
