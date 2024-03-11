/**
 * @file OS_Ctrl.h
 * @author Matthew Cockburn
 * @brief OS Ctrl contains the allocation of the RTOS resources
 * including Tasks, mutexs, semaphores, queues, timers, etc
 * @version 1
 * @date 2023-09-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _INC_OS_CTRL_H_
#define _INC_OS_CTRL_H_
/* Headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* Macros */
// Task Config
#define WATCHDOG_PRI	configMAX_PRIORITIES - 1
#define IMU_PRI         tskIDLE_PRIORITY + 2
#define PRESSURE_PRI    tskIDLE_PRIORITY + 1

#define IMU_STACK_SIZE          configMINIMAL_STACK_SIZE + 100
#define PRESSURE_STACK_SIZE     configMINIMAL_STACK_SIZE + 100
// Queue Config
#define IMU_QUEUE_SIZE     200
#define PRESSURE_QUEUE_SIZE 200

/* Extrenal Variables */
extern QueueHandle_t     ROSReaderQueue;
extern QueueHandle_t     IMU_ReaderQueue;
extern QueueHandle_t     PRESSURE_ReaderQueue;

extern SemaphoreHandle_t ROSReaderSemphr; 
extern SemaphoreHandle_t I2CCommandSemphr;
/**
 * @brief Initalize all tasks run by the RTOS kernel
 */
void OS_TasksInit(void);
/**
 * @brief Initalize all Queues used in the firmware
 */
void OS_QueuesInit(void);
/**
 * @brief Initialize all Semaphores used in the firmware
 */
void OS_SemaphoreInit(void);
/**
 * @brief Initalize all Mutexes used in the firmware
 */
void OS_MutexesInit(void);
#endif /* _INC_OS_CTRL_H_ */
