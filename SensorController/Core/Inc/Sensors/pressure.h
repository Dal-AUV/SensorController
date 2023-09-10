/**
 * @file pressure.h
 * @author Matthew Cockburn
 * @brief Pressure Sensor Header File
 * @version 1
 * @date 2023-09-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _INC_PRESSURE_H_
#define _INC_PRESSURE_H_

/* Headers */

/* Macros */

/* Data Structures */

/* Global Variables */

/* Public Prototypes */
/**
 * @brief Pressure Test Task
 * 
 * @param arguments none
 */
void PRESSURE_Task(void * arguments);
/**
 * @brief Test Task for the Pressure Task
 * 
 * @param arguments 
 */
void PRESSURE_TestTask(void *arguments);
#endif /* _INC_PRESSURE_H_ */