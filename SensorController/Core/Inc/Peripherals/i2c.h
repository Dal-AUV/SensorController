/**
 * @file i2c.h
 * @author Wyatt Shaw
 * @brief Header file for the i2c hardware interfaces
 * @version 0.1
 * @date 2023-02-13
 *
 * @copyright Copyright (c) 2023
 *
 */
/* Headers */
#include <stdarg.h>
#include "main.h"
#include "FreeRTOS.h"

#include "queue.h"
#include "semphr.h"

/* Public Variables */
extern QueueHandle_t xI2CQueue, xQueue2;

extern SemaphoreHandle_t TestMutex;

extern I2C_HandleTypeDef hi2c1;

/* Public Functions */
void i2cWrite(const char * format, ...);

void Request_Debug_Read(void);

void TASKI2CDebugParser(void);

void EnableI2CQueue(void);


