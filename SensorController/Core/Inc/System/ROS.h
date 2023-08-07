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

/* Macros and Structures */
#define MAX_PKT_LENGTH 100
#define MAX_OPCODE_LEN 2


// Protocol Opcodes
#define HEADER         "$"
#define OPCODE_START   "<"
#define OPCODE_END     ">"
#define DELIMITER      ":" 

#define OP_ARHS        "AH"

#define OP_THRUSTER_1  "T1"
#define OP_THRUSTER_2  "T2"
#define OP_THRUSTER_3  "T3"
#define OP_THRUSTER_4  "T4"
#define OP_THRUSTER_5  "T5"
#define OP_THRUSTER_6  "T6"

#define OP_TEMP_1      "C1"
#define OP_TEMP_2      "C2"

#define OP_PRESSURE_1  "P1"
#define OP_PRESSURE_2  "P2"

typedef struct AHRS_Pkt 
{
    float yaw;
    float pitch;
    float roll;
}AHRS_Pkt_t;

typedef struct Temp_Pkt
{
    uint8_t id;
    float temp;
}Temp_Pkt_t;

typedef struct Pressure_Pkt
{
    uint8_t id;
    float pressure;
}Pressure_Pkt_t;

typedef union GenericPkt
{
    AHRS_Pkt_t      ahrs;
    Temp_Pkt_t      temp;
    Pressure_Pkt_t  pressure;
}GenericPkt_t;

struct Node{
    GenericPkt_t pkt;
    enum id;
    struct Node* next;
}

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
#endif /* __INC_ROS_HANDLER_H_ */
