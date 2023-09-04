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

/* Macros */
#define MAX_PKT_LENGTH 80
/* End of Macros */

/* Structures */
/// @brief Packet Opcodes
typedef enum ROS_PktId_e
{
    ROS_ACK         = 0x00,
    ROS_Thurster    = 0x01,
    ROS_AHRS        = 0x02,
    ROS_Temperature = 0x03,
    ROS_Pressure    = 0x04,
    
    BAD_PKT         = 0xFF,
    
}ROS_PktId_t;
/// Packet Structures
typedef struct Thruster_Pkt_s
{
    uint8_t opcode;
    uint32_t t1;
    uint32_t t2;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint8_t  csum;

}Thruster_Pkt_t;
typedef struct AHRS_Pkt_s
{
    uint8_t opcode;
    float yaw;
    float pitch;
    float roll;
    uint8_t csum;

}AHRS_Pkt_t;

typedef struct Temp_Pkt_s
{
    uint8_t opcode;
    uint8_t id;
    float temp;
    uint8_t csum;

}Temp_Pkt_t;

typedef struct Pressure_Pkt_s
{
    uint8_t opcode;
    float pressure;
    uint8_t csum;

}Pressure_Pkt_t;

typedef struct GenericPkt_s
{
    ROS_PktId_t id;
    union
    {
        Thruster_Pkt_t  thruster;
        AHRS_Pkt_t      ahrs;
        Temp_Pkt_t      temp;
        Pressure_Pkt_t  pressure;
        uint8_t         buffer[MAX_PKT_LENGTH];
    }pkt;
}GenericPkt_t;

/// Packet Size Dictionary
typedef enum ROSDecoder_e
{
    ROS_Decoder_Disable,
    ROS_Decoder_Header,
    ROS_Decoder_Data,

}ROSDecoder_t;
typedef struct ROS_Decoder_s
{
    
    uint8_t csum;
    uint8_t pkt_len;
    uint8_t cur_len;
    ROSDecoder_t state;

}ROS_Decoder_t;

typedef struct ROS
{
    
    ROS_Decoder_t decoder;
    GenericPkt_t rx;
    GenericPkt_t tx;
        
}ROS_t;

/* Public Variables */
extern QueueHandle_t ROS_Writer_Queue;
/* Public Prototypes */
/**
 * @brief The Reader Task for the ROS Interface
 * 
 * @param arguments (not handled)
 */
void ROS_ReaderTask(void * arguments);
/**
 * @brief The Writer Task for the ROS Interface
 * 
 * @param arguments (not handled)
 */
void ROS_WriterTask(void * arguments);

void ROS_Init(void);

void ROS_EnableDecoder(void);

void ROS_DisableDecoder(void);

uint8_t ROS_GetPktLen(ROS_PktId_t id);
#endif /* __INC_ROS_HANDLER_H_ */

