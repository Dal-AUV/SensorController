/**
 * @file ROS.c
 * @author Matthew Cockburn 
 * @brief 
 * @version 0.1
 * @date 2023-06-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "System/ROS.h"
#include "System/OS_Ctrl.h"
#include "Interfaces/ros_if.h"
#include "Peripherals/usart.h"

/* Macros */
#define ROS_DECODER_SLEEP_MS  30000 // 30 Seconds
#define ROS_TIME_OUT_MS       1000  // 1 Second
/* End of Macros */

/* Local Global Variables */
ROS_t ROS;

const ROS_Callbacks_t ROS_CallbackTable[]={
	{.op = DAT_OP_STATUS,   .func = ROS_StatusCallback},
	{.op = DAT_OP_CTRL,     .func = ROS_ControlCallback},
	{.op = DAT_OP_AHRS,     .func = ROS_DummyCallback},
	{.op = DAT_OP_IMU,      .func = ROS_DummyCallback},
	{.op = DAT_OP_TEMP,     .func = ROS_DummyCallback},
	{.op = DAT_OP_PRES,     .func = ROS_DummyCallback},
    {.op = DAT_OP_THRU,     .func = ROS_DummyCallback},
	{.op = DAT_OP_ACK,      .func = ROS_DummyCallback},
};

const DAT_Pkt_Dictionary_t DAT_ProtocolDictionary[] ={
	{.op = DAT_OP_STATUS,   .sz = sizeof(DAT_StatusPkt_t)},
	{.op = DAT_OP_CTRL,     .sz = sizeof(DAT_ControlPkt_t)},
	{.op = DAT_OP_AHRS,     .sz = sizeof(DAT_AHRSPkt_t)},
	{.op = DAT_OP_IMU,      .sz = sizeof(DAT_IMUPkt_t)},
	{.op = DAT_OP_TEMP,     .sz = sizeof(DAT_TemperaturePkt_t)},
	{.op = DAT_OP_PRES,     .sz = sizeof(DAT_PressurePkt_t)},
    {.op = DAT_OP_THRU,     .sz = sizeof(DAT_ThrustersPkt_t)},
	{.op = DAT_OP_ACK,      .sz = sizeof(DAT_AckPkt_t)},
};
/* End of Global Variables */

/* Private Prototypes */
void ROS_Dispatcher(void);
void ROS_DecoderReset(void);
bool ROS_CheckOpcode(uint8_t * byte);
void ROS_CalculateCSUM(DAT_GenericPkt_t * pkt, uint8_t len);
bool ROS_VerifyCSUM(DAT_GenericPkt_t * pkt, uint8_t len);
/* End Of Prototypes */

/* RTOS Tasks */
void ROS_ReaderTask(void * arguments)
{
    uint8_t byte = 0;
    while(1){
        
        /* Grab Messages From the Serial Interface */
        if(pdFAIL == xQueueReceive(ROS_ReaderQueue,&byte,pdMS_TO_TICKS(ROS_TIME_OUT_MS))){
            ROS_DecoderReset();
        }
        switch(ROS.decoder.state)
        {
        case ROS_Decoder_Header:
            if(ROS_CheckOpcode(&byte)){
                // Error, reset decoder
                ROS_DecoderReset();
            }else{
                ROS.decoder.csum ^= byte;
                ROS.rx.pkt.buffer[ROS.decoder.cur_len++] = byte;
                ROS.decoder.state = ROS_Decoder_Data;
            }
        break;

        case ROS_Decoder_Data:

            ROS.rx.pkt.buffer[ROS.decoder.cur_len++] = byte;
            if(ROS.decoder.cur_len == ROS.rx.meta.sz){
                if(ROS.decoder.csum == byte){
                	ROS_Dispatcher();
            	}
                ROS_DecoderReset();
            }else{
            	ROS.decoder.csum ^= byte;
            }

        break;
        
        case ROS_Decoder_Disable:
            vTaskDelay(pdMS_TO_TICKS(ROS_DECODER_SLEEP_MS));
            break;

        default:
            // Error, reset decoder
            ROS_DecoderReset();
        }
        /* If no messages are in the queue than sleep*/
        if((UBaseType_t)0 == uxQueueMessagesWaiting(ROS_ReaderQueue)){
            vTaskDelay(pdMS_TO_TICKS(ROS_TIME_OUT_MS));
        }
    }
}

void ROS_WriterTask(void * arguments){

    while(1){

        if(pdFAIL == xQueueReceive(ROS_WriterQueue,&ROS.tx, portMAX_DELAY)){
            continue;
        }
        // Send Packet through ROS UART
        ROS_CalculateCSUM(&ROS.tx.pkt,ROS.tx.meta.sz);
        ROS_Write(ROS.tx.pkt.buffer,ROS.tx.meta.sz,portMAX_DELAY);
    }
}
/* End Of RTOS Tasks */

/* Public Functions*/
void ROS_EnableDecoder(void){
    ROS_DecoderReset();
}

void ROS_DisableDecoder(void){
    
    ROS.decoder.csum    = 0;
    ROS.decoder.pkt_len = 0;
    ROS.decoder.cur_len = 0;
    ROS.decoder.state   = ROS_Decoder_Disable;

}

/* End of Public Functions*/

/**
 * @brief Function to be called once a valid DAT ROS packet 
 * has been received, will call the corresponding Callback function 
 * defined by the driver developer 
 */
void ROS_Dispatcher(void){

	for (uint8_t i = 0; i < DAT_PROTOCOL_SIZE; ++i){
		if(ROS.decoder.op == ROS_CallbackTable[i].op){
			ROS_CallbackTable[i].func();
		}
	}

    return;
}
/**
 * @brief Calculate the XOR Checksum of a DAT ROS Packet
 * @param pkt - pointer to the packet data structure 
 * @param len length of packet
 */
void ROS_CalculateCSUM(DAT_GenericPkt_t * pkt, uint8_t len){
    for(uint8_t i = 0; i<(len-1); ++i){
        pkt->buffer[len-1] ^= pkt->buffer[i];
    }
    return;
}
/**
 * @brief Verify that the check sum is valid
 * @param pkt pointer to the packet
 * @param len length of the packet
 * @return true Success
 * @return false Invalid
 */
bool ROS_VerifyCSUM(DAT_GenericPkt_t * pkt, uint8_t len){
    uint8_t csum = 0;
    for(uint8_t i = 0; i<len-1; ++i){
        csum ^= pkt->buffer[i];
    }
    if(csum != pkt->buffer[len-1]){
        return 1;
    }else{
        return 0;
    }
}
/**
 * @brief Resets the ROS Decoder state machine
 */
void ROS_DecoderReset(void){
    
    ROS.decoder.csum    = 0;
    ROS.decoder.pkt_len = 0;
    ROS.decoder.cur_len = 0;
    ROS.decoder.state   = ROS_Decoder_Header;

    return;
}
/**
 * @brief Checks the received opcode has is valid, if valid will update
 * the ROS.Rx Transport Meta Data for preceding decoding
 * @param  byte read from UART
 * @return true -  Success
 * @return false - Failure
 */
bool ROS_CheckOpcode(uint8_t * byte){

	assert(byte);

    for(uint8_t i =0; i < DAT_PROTOCOL_SIZE; ++i){
    	if (*byte == DAT_ProtocolDictionary[i].op){
    		memcpy(&ROS.rx.meta,&DAT_ProtocolDictionary[i], sizeof(DAT_Pkt_Dictionary_t));
    		return true;
    	}
    }
    return false;
}

void ROS_StatusCallback(void){

	ROS_Transport_t msg;
	msg.pkt.ackp.op = DAT_OP_ACK;

	xQueueSendToBack(ROS_WriterQueue,&msg,0);

	return;
}

void ROS_ControlCallback(void){

	ROS_Transport_t msg;
	msg.pkt.ackp.op = DAT_OP_ACK;

	xQueueSendToBack(ROS_WriterQueue,&msg,0);

	return;
}

void ROS_DummyCallback(void){
	return;
}
 /* End of Private Functions */
/// EOF
