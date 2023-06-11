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
#include <stdarg.h>
#include "main.h"
#include "FreeRTOS.h"

#include <stdbool.h>

#include "queue.h"
#include "semphr.h"
/* Definitions */
#define MAX_USART_BUF_SIZE 50
#define MAX_USART_QUEUE_SIZE 80
typedef enum UART_STATUS{
    UART_STATUS_IDLE,
    UART_STATUS_READ,
    UART_STATUS_WRITE

}UART_STATUS_t;

typedef enum UART_SENSORS{
	SENSOR1,
	SENSOR2,
	SENSOR3

}UART_SENSORS_t;

extern UART_HandleTypeDef huart3;

/* TypeDefs and Structs */
/**
 * @brief 
 * 
 */
typedef struct UART_Interface{
    
    UART_HandleTypeDef *uart_h;

    QueueHandle_t      *queue_h;
    SemaphoreHandle_t  *sem_tx;
    SemaphoreHandle_t  *sem_rx;
    bool                init;
    UART_STATUS_t       status;
    UART_SENSORS_t 		sensors; //this is a potential implementation to experiment with

}DAT_USART_Handle_t;

/* Public Functions */
/**
 * @brief Function call to initialize a uart interface
 * The user will need to provide the the uart handle interface that is allocated
 * by the user before calling
 * @param handle 
 */
void UART_Init(DAT_USART_Handle_t * handle);
/**
 * @brief Function to deinitialize a uart interface
 * 
 * @param handle 
 */
void UART_DeInit(DAT_USART_Handle_t * handle);
/**
 * @brief Write data to a uart interface
 * 
 * @param handle 
*/
void UART_Write(DAT_USART_Handle_t * handle, uint8_t * buf, uint16_t length);
/* Public Functions */
/**
 * @brief Function to enable the uart isr callbacks
 * 
 */
void EnableDebug(void);
/**
 * @brief 
 * 
 */
void Request_Debug_Read(void);
/**
 * @brief 
 * 
 * @param format 
 * @param ... 
 */
void DebugWrite(const char * format, ...);
/**
 * @brief 
 * 
 * @param format 
 * @param ... 
 */
void DebugWriteUnprotected(const char * format, ...);
/**
 * @brief 
 * 
 */
void TASKDebugParser(void);

