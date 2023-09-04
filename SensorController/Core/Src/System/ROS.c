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
#include "System/ROS.h"
#include <assert.h>
#include <string.h>
#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "System/TaskCtrl.h"
#include "System/ROS.h"
#include "Peripherals/usart.h"

/* Macros */
#define ROS_DECODER_SLEEP_MS  60000 // 1 minute
/* End of Macros */

/* External Global Variables */
QueueHandle_t ROS_Writer_Queue;

/* Local Global Variables */
ROS_t ROS;
/* End of Global Variables */

/* Private Prototypes */
void ROS_Decoder_Reset(void);
uint8_t ROS_Check_Opcode(uint8_t * byte);
/* End Of Prototypes */

/* RTOS Tasks */
void ROS_Reader_Task(void * arguments)
{
    char packet[MAX_PKT_LENGTH];
    uint8_t byte = 0;

    ROSDecoder_t state = ROS_Decoder_Header;
    while(1){
        if(pdFAIL == xQueueReceive(ROSQueue,&byte,portMAX_DELAY)){
            ROS_Decoder_Reset();
        }
        switch(ROS.decoder.state)
        {
        case ROS_Decoder_Header:
            if(ROS_Check_Opcode(&byte)){
                // Error, reset decoder
                ROS_Decoder_Reset();
            }else{
                ROS.decoder.csum ^= byte;
                ROS.rx.pkt.buffer[ROS.decoder.cur_len++];
                ROS.decoder.state = ROS_Decoder_Data;
            }
        break;

        case ROS_Decoder_Data:
            ROS.decoder.csum ^= byte;
            ROS.rx.pkt.buffer[ROS.decoder.cur_len++];
            if(ROS.decoder.cur_len == ROS.decoder.pkt_len){
                ROS_Dispatcher();
                ROS_Decoder_Reset();
            }
        break;
        
        case ROS_Decoder_Disable:
            vTaskDelay(pdMS_TO_TICKS(ROS_DECODER_SLEEP_MS));
            break;

        default:
            // Error, reset decoder
            ROS_Decoder_Reset();
        }   
    }

}

ROS_Writer_Task(void * arguments){
    
    while(1){

        if(pdFAIL == xQueueReceive(ROS_Writer_Queue,&ROS.tx, portMAX_DELAY)){
            continue;
        }

    }
}
/* End Of RTOS Tasks */

/* Public Functions*/
void ROS_Enable_Decoder(void){
    ROS_Decoder_Reset();
}

void ROS_Disable_Decoder(void){
    
    ROS.decoder.csum    = 0;
    ROS.decoder.pkt_len = 0;
    ROS.decoder.cur_len = 0;
    ROS.decoder.state   = ROS_Decoder_Disable;

}

/* End of Public Functions*/

/* Private Functions */
void ROS_Dispatcher(void){

}

void ROS_Calculate_CSUM(GenericPkt_t * pkt, uint8_t len){
    pkt->pkt.buffer[0] = 0;
    for(uint8_t i = 0; i<(len-1); ++i){
        pkt->pkt.buffer[len-1] ^= pkt->pkt.buffer[i];
    }
    return;
}

uint8_t ROS_Verify_CSUM(GenericPkt_t * pkt, uint8_t len){
    uint8_t csum = 0;
    for(uint8_t i = 0; i<len-1; ++i){
        csum ^= pkt->pkt.buffer[i];
    }
    if(csum != pkt->pkt.buffer[len-1]){
        return 1;
    }else{
        return 0;
    }
}

void ROS_Decoder_Reset(void){
    
    ROS.decoder.csum    = 0;
    ROS.decoder.pkt_len = 0;
    ROS.decoder.cur_len = 0;
    ROS.decoder.state   = ROS_Decoder_Header;

    return;
}

uint8_t ROS_Check_Opcode(uint8_t * byte){
    assert(byte);
    uint8_t rc = 0;

    switch(*byte){
        case ROS_Thurster:
            ROS.rx.opcode = ROS_Thurster;
            ROS.decoder.pkt_len = sizeof(Thruster_Pkt_t);
            break;
        case ROS_AHRS:
            ROS.rx.opcode = ROS_AHRS;
            ROS.decoder.pkt_len = sizeof(AHRS_Pkt_t);
            break;
        case ROS_Temperature:
            ROS.rx.opcode = ROS_Temperature;
            ROS.decoder.pkt_len = sizeof(Temp_Pkt_t);
            break;
        case ROS_Pressure:
            ROS.rx.opcode = ROS_Pressure;
            ROS.decoder.pkt_len = sizeof(Pressure_Pkt_t);
            break;
        default:
            ROS.rx.opcode = BAD_PKT;
            rc = 1;
            break;
    }

    return rc;
}
 /* End of Private Functions */