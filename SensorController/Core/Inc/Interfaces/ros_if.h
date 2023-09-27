/**
 * @file ros_if.h
 * @author Matthew Cockburn 
 * @brief ROS interface file, defines the DAT communication protocol
 * between the real time processors and SOC's. Each packet structure should be
 * defined here. 
 * @warning This file is intended to be cross-complied on both MCU and SOC targets
 * therefore now system specify headers or functions should be defined in this file 
 * or the source
 * @version 1.0
 * @date 2023-09-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef INC_INTERFACES_ROS_IF_H_
#define INC_INTERFACES_ROS_IF_H_

#define DAT_MAX_PKT_SIZE  100
#define DAT_PROTOCOL_SIZE 7

typedef enum DAT_Opcode_e
{
    DAT_OP_STATUS = 0x00,
    DAT_OP_CTRL   = 0x01,
    DAT_OP_AHRS   = 0x02,
    DAT_OP_IMU    = 0x03,
    DAT_OP_TEMP   = 0x04,
    DAT_OP_PRES   = 0x05,
    DAT_OP_THRU   = 0x06,
    //-------------------
    // Reserved 
    //-------------------
    DAT_OP_ACK    = 0xff,
}DAT_Opcode_t;

typedef struct DAT_StatusPkt_s
{
    uint8_t op;
    uint8_t battery;
    uint8_t pressure;
    uint8_t temperature;
    uint8_t csum;
} DAT_StatusPkt_t;

typedef struct DAT_ControlPkt_s
{
    uint8_t op;
    uint8_t control_mask;
    uint8_t csum;
}DAT_ControlPkt_t;

typedef struct DAT_AHRSPkt_s
{
    uint8_t op;
    uint32_t heading;
    uint32_t attitude;
    uint8_t csum;
} DAT_AHRSPkt_t;

typedef struct DAT_IMUPkt_s
{
    uint8_t op;
    uint32_t yaw;
    uint32_t pitch;
    uint32_t roll;
    uint8_t csum;
} DAT_IMUPkt_t;

typedef struct DAT_TemperaturePkt_s
{
    uint8_t op;
    uint32_t yaw;
    uint32_t pitch;
    uint32_t roll;
    uint8_t csum;
} DAT_TemperaturePkt_t;

typedef struct DAT_PressurePkt_s
{
    uint8_t op;
    uint16_t pressure;
    uint8_t csum;
} DAT_PressurePkt_t;

typedef struct DAT_ThrustersPkt_s
{
    uint8_t  op;
    uint16_t t1;
    uint16_t t2;
    uint16_t t3;
    uint16_t t4;
    uint16_t t5;
    uint16_t t6;
    uint8_t  csum;
}DAT_ThrustersPkt_t;
typedef struct DAT_AckPkt_s
{
    uint8_t op;
    uint8_t csum;
} DAT_AckPkt_t;

typedef union DAT_GenericPkt_u
{
    uint8_t buffer[DAT_MAX_PKT_SIZE];

    DAT_StatusPkt_t         stat;
    DAT_ControlPkt_t        ctrl;
    DAT_AHRSPkt_t           ahrs;
    DAT_IMUPkt_t            imup;
    DAT_TemperaturePkt_t    temp;
    DAT_PressurePkt_t       pres;
    DAT_AckPkt_t            ackp;
    DAT_ThrustersPkt_t      thru;
    
}DAT_GenericPkt_t;

typedef struct DAT_Pkt_Dictionary_s
{
    DAT_Opcode_t op;
    uint16_t     sz;
}DAT_Pkt_Dictionary_t;

#endif /* INC_INTERFACES_ROS_IF_H_ */
