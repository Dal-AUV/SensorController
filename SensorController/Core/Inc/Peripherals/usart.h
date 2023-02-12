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

#include "queue.h"

/* Definitions */
#define MAX_USART_BUF_SIZE 50
#define MAX_USART_QUEUE_SIZE 80

/* TypeDefs and Structs */
// typedef USART_Handle{
//     UART_HandleTypeDef  usart_h,
//     QueueHandle_t       queue_h,
//     SemaphoreHandle_t   sema_h,

// }DAT_USART_Handle_t;

/* Public Variables */
extern QueueHandle_t DebugQueue;


/* Public Functions */
/**
 * @brief Function to enable the debug usart isr callbacks
 * 
 */
void EnableDebug(void);
/**
 * @brief 
 * 
 */
void Request_Debug_Read(void)
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

