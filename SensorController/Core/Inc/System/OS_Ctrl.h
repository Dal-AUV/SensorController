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
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Extrenal Variables */
extern QueueHandle_t     ROSReaderQueue;
extern SemaphoreHandle_t ROSReaderSemphr; 
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