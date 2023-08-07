/**
 * @file ROS_Dictionary.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef INC_SYSTEM_ROS_DICTIONARY_H_
#define INC_SYSTEM_ROS_DICTIONARY_H_
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

#define NUM_OF_OPCODES 11
#define OPCODE_SIZE    2

extern char opcode_dictionary[NUM_OF_OPCODES][OPCODE_SIZE];

typedef enum PKT_ID
{
    AHRS_PKT,
    THRUSTER_1,
    THRUSTER_2,
    THRUSTER_3,
    THRUSTER_4,
    THRUSTER_5,
    THRUSTER_6,
    TEMP_1,
    TEMP_2,
    PRESSURE_1,
    PRESSURE_2,

    BAD_PKT
    
}ePKT_ID_t;
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

typedef struct GenericPkt
{
    union
    {
        AHRS_Pkt_t      ahrs;
        Temp_Pkt_t      temp;
        Pressure_Pkt_t  pressure;
    }pkt;
    ePKT_ID_t id;
}GenericPkt_t;

#endif /* INC_SYSTEM_ROS_DICTIONARY_H_ */
