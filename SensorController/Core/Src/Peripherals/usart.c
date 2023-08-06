/**
 * @file usart.c
 * @author Matthew Cockburn & Wyatt Davion
 * @brief Source file for the USART hardware interfaces, contains ISR 
 * implementation, Receive and Transmit functions
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
// HAL UART DMA Handles
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

// UART Queues
QueueHandle_t ROSQueue;
// UART Interface Locks
SemaphoreHandle_t ROSMutex;
// Receive Buffers
uint8_t ROSBuf[MAX_USART_BUF_SIZE];

/* Private Prototypes*/
void MX_USART3_UART_Init(void);
void MX_USART2_UART_Init(void);
HAL_StatusTypeDef UART_DMA_Write(UART_HandleTypeDef *huart, uint8_t * buf, uint16_t size,
		uint32_t timeout);
/* Exported Functions */

void UART_Init(void)
{
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
}

HAL_StatusTypeDef ROS_Write(uint8_t * buffer, uint16_t size, uint32_t timeout)
{
	return UART_DMA_Write(&huart3, buffer,size,timeout);
}

/*  USART CALLBACKS */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

    BaseType_t xStatus ={0};

    //sanity check pin toggle to see if Callback is running will flash red light
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

    if(huart == &huart3){

        xStatus = xQueueSendToBackFromISR(ROSQueue,ROSBuf,NULL);
        HAL_UART_Receive_IT(&huart3, ROSBuf, UART_RX_ISR_TRIGGER_SZ);
    }
    if(xStatus == pdPASS){
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    }
    return;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if(huart == &huart3){
		xSemaphoreGiveFromISR(ROSMutex,NULL);
	}

	return;
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
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
  /* USER CODE BEGIN USART3_Init 2 */
  ROSQueue = xQueueCreate(MAX_USART_QUEUE_SIZE, sizeof(uint8_t));
  ROSMutex = xSemaphoreCreateBinary();
  xSemaphoreGive(ROSMutex);
  HAL_UART_Receive_IT(&huart3, ROSBuf, UART_RX_ISR_TRIGGER_SZ);
  /* USER CODE END USART3_Init 2 */

}

HAL_StatusTypeDef UART_DMA_Write(UART_HandleTypeDef *huart, uint8_t * buf, uint16_t size,
		uint32_t timeout)
{

	assert(huart);
	assert(buf);

	if(size == 0) return HAL_OK;

	if(!xSemaphoreTake(ROSMutex, pdMS_TO_TICKS(timeout))){
		return HAL_TIMEOUT;
	}

	return HAL_UART_Transmit_DMA(huart, buf, size);
}
