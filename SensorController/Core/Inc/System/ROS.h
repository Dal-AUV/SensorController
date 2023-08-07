/**
 * @file RosHandler.h
 * @author Matthew Cockburn
 * @brief Header file for the ROS Interface
 * @version 0.1
 * @date 2023-08-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __INC_ROS_HANDLER_H_
#define __INC_ROS_HANDLER_H_

/* Headers */
#include "FreeRTOS.h"
#include "queue.h"
#include "ROS_Dictionary.h"
/* Macros and Structures */
#define MAX_PKT_LENGTH 100
#define MAX_OPCODE_LEN 2

/* Public Variables */

/* Public Prototypes */
/**
 * @brief The Reader Task for the ROS Interface
 * 
 * @param arguments (not handled)
 */
void ROS_Reader(void * arguments);
/**
 * @brief The Writer Task for the ROS Interface
 * 
 * @param arguments (not handled)
 */
void ROS_Writer(void * arguments);
/**
 * @brief Get the Opcode Id object
 * 
 * @param code 
 * @param size 
 * @return ePKT_ID_t 
 */
ePKT_ID_t GetOpcodeId(char* code, uint16_t size);
#endif /* __INC_ROS_HANDLER_H_ */

