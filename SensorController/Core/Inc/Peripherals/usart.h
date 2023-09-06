/**
 * @file usart.h
 * @author Matthew Cockburn
 * @brief Header file for the USART hardware interfaces
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/* Headers */
#include "main.h"
#include "FreeRTOS.h"

#include <stdbool.h>

#include "queue.h"
#include "semphr.h"
/* Definitions */
#define MAX_USART_BUF_SIZE 50
#define MAX_USART_QUEUE_SIZE 80
#define UART_RX_ISR_TRIGGER_SZ 1

// Add your Sensor Name Here
typedef enum UART_SENSORS{
	UART_ROS,
    
	SENSOR_TOTAL

}UART_SENSORS_t;

/* Public Functions */
/**
 * @brief Initializes all defined UART interfaces
 * 
 * @return HAL_StatusTypeDef 
 */
void UART_Init(void);
/**
 * @brief UART and DMA Write Wrapper for the ROS interface.
 * Starts a DMA transfer for ROS interface
 * @param buffer, pointer to the buffer of data to be written
 * @param size, size of the buffer to be written
 * @param timeout, time the function call will until a timeout occurs
 * @return HAL_StatusTypeDef, HAL_OK on success, HAL_TIMEOUT on timeout. 
 */
HAL_StatusTypeDef ROS_Write(uint8_t * buffer, uint16_t size, uint32_t timeout);


