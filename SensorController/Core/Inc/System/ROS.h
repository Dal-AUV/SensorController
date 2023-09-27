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

#include "Interfaces/ros_if.h"


#define ROS_WRITER_QUEUE_LEN 5

typedef enum ROSDecoder_e
{
    ROS_Decoder_Disable,
    ROS_Decoder_Header,
    ROS_Decoder_Data,

}ROSDecoder_t;

typedef struct ROS_Decoder_s
{
    uint8_t 		csum;
    uint8_t 		pkt_len;
    uint8_t 		cur_len;
    DAT_Opcode_t 	op;
    ROSDecoder_t 	state;

}ROS_Decoder_t;

typedef struct ROS_Transport_s
{
	DAT_Pkt_Dictionary_t meta;
	DAT_GenericPkt_t	 pkt;
}ROS_Transport_t;

typedef struct ROS_s
{
    ROS_Decoder_t decoder;
    ROS_Transport_t rx;
    ROS_Transport_t tx;
        
}ROS_t;

typedef struct ROS_Callbacks_s
{
	DAT_Opcode_t 	op;
	void 			(*func)(void);
}ROS_Callbacks_t;

/* Public Variables */

/* Public Prototypes */
/**
 * @brief The Reader Task for the ROS Interface,
 * This tasks uses the ROS Decoder State Machine 
 * that is serviced every 30 seconds
 * @param arguments (not handled)
 */
void ROS_ReaderTask(void * arguments);
/**
 * @brief The Writer Task for the ROS Interface
 * This task will write outgoing ROS messages over 
 * the serial interface. This tasks services the 
 * ROS_WriterQueue immediately after a messages is 
 * written to the queue. 
 * @param arguments (not handled)
 */
void ROS_WriterTask(void * arguments);
/**
 * @brief Function to initialize the ROS Decoder
 */
void ROS_Init(void);
/**
 * @brief  Function to enable the ROS Decoder
 */
void ROS_EnableDecoder(void);
/**
 * @brief Function to disable the ROS Decoder
 */
void ROS_DisableDecoder(void);
/**
 * @brief Callback function used for the ROS Status Packet
 */
void ROS_StatusCallback(void);
/**
 * @brief Callback function used for the ROS Control Packet
 */
void ROS_ControlCallback(void);
/**
 * @brief Callback function used for undefined ROS packets
 */
void ROS_DummyCallback(void);

#endif /* __INC_ROS_HANDLER_H_ */

