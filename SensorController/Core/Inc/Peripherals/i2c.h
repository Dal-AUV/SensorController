/**
 * @file i2c.h
 * @author Wyatt Shaw
 * @brief Header file for the I2C hardware interfaces
 * @version 0.1
 * @date 2023-06-27
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

/* Definitions*/
#define MAX_USART_BUF_SIZE 50
#define MAX_USART_QUEUE_SIZE 80
extern I2C_HandleTypeDef hi2c1;

typedef enum I2C_SENSORS {
    I2C_Sensor1,
    I2C_Totals
}I2C_SENSORS_t;


typedef struct I2C_Interface{

    I2C_HandleTypeDef   *i2c_h;
    I2C_TypeDef         *MEM_BASE;
    QueueHandle_t      	queue_h; //changed from pointer to non pointer
    uint8_t*			buf;
    SemaphoreHandle_t  	sem_tx;
    SemaphoreHandle_t  	sem_rx;
    bool                initFlag;
    I2C_SENSORS_t       sensors;
    uint32_t            OWN_ADDRESS;
    uint32_t            DEV_ADDRESS;



}DAT_I2C_HANDLE_t;

/* Functions*/


HAL_StatusTypeDef I2C_Init(void);
/**
 * @brief iterates through to initialize each sensor 
 * 
 * @param ...
 * 
*/


HAL_StatusTypeDef I2C_Init(struct I2C_Interface * handle);
/**
 * @brief Function call to initialize the i2c interfaces
 * 
 * 
 * @param handle 
 */