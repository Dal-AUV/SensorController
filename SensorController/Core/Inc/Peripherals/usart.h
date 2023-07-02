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

// UART HAL Interfaces are defined here
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

// Add your Sensor Name Here
typedef enum UART_SENSORS{
	ROS,
    
	SENSOR_TOTAL

}UART_SENSORS_t;

/* TypeDefs and Structs */
/**
 * @brief UART API Interface Struct for DAT Sensor 
 * connections that use UART. Provide structure with all
 * allocated variables. Reading is done via the queue_h struct.
 */
typedef struct UART_Interface{
    UART_HandleTypeDef 	*uart_h; // ptr to HAL layer
    USART_TypeDef		*h_addr; // ptr to peripheral mem
    QueueHandle_t      	queue_h; // input queue
    uint8_t*			buf;    // add to make generic uart func work
    SemaphoreHandle_t  	sem_tx;
    SemaphoreHandle_t  	sem_rx;
    bool               	initFlag;
    UART_SENSORS_t 	   	sensors; //this is a potential implementation to experiment with
    HAL_StatusTypeDef (*init)(struct UART_Interface*);
    HAL_StatusTypeDef (*deinit)(struct UART_Interface*);
    HAL_StatusTypeDef (*enable)(struct UART_Interface*);
    HAL_StatusTypeDef (*disable)(struct UART_Interface*);
    HAL_StatusTypeDef (*write)(struct UART_Interface*,uint8_t*,uint16_t);
    HAL_StatusTypeDef (*write_unpro)(struct UART_Interface*,uint8_t*,uint16_t);

}DAT_USART_Handle_t;

extern DAT_USART_Handle_t uarts[SENSOR_TOTAL];
/* Public Functions */
/**
 * @brief Initalizes all defined UART interfaces
 * 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef Sys_UART_Init(void);

/**
 * @brief Function call to initialize a uart interface
 * The user will need to provide the the uart handle interface that is allocated
 * by the user before calling
 * @param handle , UART API handle
 */
HAL_StatusTypeDef UART_Init(struct UART_Interface * handle);
/**
 * @brief Generic de-initialize function for uart interface,
 * this will de-init the low level HAL API's. Should not be called
 * if interface comms are needed at a later stage
 * 
 * @param handle, UART API handle
 */
HAL_StatusTypeDef UART_DeInit(struct UART_Interface * handle);

/**
 * @brief Generic UART enable function, enables the global,
 * Receive ISR callback for the low level HAL API 
 * @param handle 
 */
HAL_StatusTypeDef UART_Enable(struct UART_Interface * handle);
/**
 * @brief Generic UART disable function, disables the
 * global receive ISR for the low level HAL API
 * @param handle 
 */
HAL_StatusTypeDef UART_Disable(struct UART_Interface * handle);
/**
 * @brief Generic Write for UART interfaces, will write the
 * buffer provided (Thread Safe)
 * @param handle UART API Handle
 * @param buf buffer of data to be written
 * @param length size of the buffer
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef UART_Write(struct UART_Interface* handle, const uint8_t * buf, const uint16_t size);
/**
 * @brief Generic Write for UART interfaces, will write the
 * buffer provided (Not Thread Safe)
 * @param buf 
 * @param size 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef UART_Write_Unprotected(struct UART_Interface * handle, const uint8_t * buf,const uint16_t size);
