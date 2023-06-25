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

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

typedef enum UART_STATUS{
    UART_STATUS_IDLE,
    UART_STATUS_READ,
    UART_STATUS_WRITE

}UART_STATUS_t;

typedef enum UART_SENSORS{
	SENSOR1,
	SENSOR_TOTAL

}UART_SENSORS_t;


/* TypeDefs and Structs */
/**
 * @brief 
 * 
 */

typedef struct UART_Interface{
    
    UART_HandleTypeDef 	*uart_h;
    USART_TypeDef		*MEM_BASE;
    QueueHandle_t      	queue_h; //changed from pointer to non pointer
    uint8_t*			buf;
    SemaphoreHandle_t  	sem_tx;
    SemaphoreHandle_t  	sem_rx;
    bool               	initFlag;
    UART_SENSORS_t 	   	sensors; //this is a potential implementation to experiment with

    // This is the general format for the function pointers
    HAL_StatusTypeDef (*init)(struct UART_Interface*);
    void (*enable)(struct UART_Interface*);


}DAT_USART_Handle_t;


/* Public Functions */
HAL_StatusTypeDef Sys_UART_Init(void);

/**
 * @brief Function call to initialize a uart interface
 * The user will need to provide the the uart handle interface that is allocated
 * by the user before calling
 * @param handle 
 */
HAL_StatusTypeDef UART_Init(DAT_USART_Handle_t * handle);
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

