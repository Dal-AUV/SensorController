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
#include "System/OS_Ctrl.h"
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
void ROS_Dispatcher(void);
void ROS_DecoderReset(void);
uint8_t ROS_CheckOpcode(uint8_t * byte);
/* End Of Prototypes */

/* RTOS Tasks */
void ROS_ReaderTask(void * arguments)
{
    uint8_t byte = 0;
    while(1){
        if(pdFAIL == xQueueReceive(ROSReaderQueue,&byte,portMAX_DELAY)){
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
            ROS.decoder.csum ^= byte;
            ROS.rx.pkt.buffer[ROS.decoder.cur_len++] = byte;
            if(ROS.decoder.cur_len == ROS.decoder.pkt_len){
                ROS_Dispatcher();
                ROS_DecoderReset();
            }
        break;
        
        case ROS_Decoder_Disable:
            vTaskDelay(pdMS_TO_TICKS(ROS_DECODER_SLEEP_MS));
            break;

        default:
            // Error, reset decoder
            ROS_DecoderReset();
        }   
    }

}

void ROS_WriterTask(void * arguments){
    
    while(1){

        if(pdFAIL == xQueueReceive(ROS_Writer_Queue,&ROS.tx, portMAX_DELAY)){
            continue;
        }
        // Send Packet through ROS UART
        ROS_Calculate_CSUM(&ROS.tx,sizeof(ROS.tx.id));
        ROS_Write(ROS.tx.pkt.buffer,(uint16_t)ROS_GetPktLen(ROS.tx.id),portMAX_DELAY);
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

uint8_t ROS_GetPktLen(ROS_PktId_t id){
    uint8_t size = 0;
    
    switch (id)
    {
    case ROS_Thurster:
        size = sizeof(Thruster_Pkt_t);
        break;
    case ROS_AHRS:
        size = sizeof(AHRS_Pkt_t);
        break;
    case ROS_Temperature:
        size = sizeof(Temp_Pkt_t);
        break;
    case ROS_Pressure:
        size = sizeof(Pressure_Pkt_t); 
    default:
        break;
    }
    return size;
}
/* End of Public Functions*/

/* Private Functions */
/**
 * @brief Dispatch the command to proper sensor interface
 * 
 */
void ROS_Dispatcher(void){
    // TODO Determine how command gets dispatched
    // Echoing packet
    ROS_Write(ROS.rx.pkt.buffer,(uint16_t)ROS_GetPktLen(ROS.rx.id),portMAX_DELAY);
    return;
}

void ROS_CalculateCSUM(GenericPkt_t * pkt, uint8_t len){
    pkt->pkt.buffer[0] = 0;
    for(uint8_t i = 0; i<(len-1); ++i){
        pkt->pkt.buffer[len-1] ^= pkt->pkt.buffer[i];
    }
    return;
}

uint8_t ROS_VerifyCSUM(GenericPkt_t * pkt, uint8_t len){
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

void ROS_DecoderReset(void){
    
    ROS.decoder.csum    = 0;
    ROS.decoder.pkt_len = 0;
    ROS.decoder.cur_len = 0;
    ROS.decoder.state   = ROS_Decoder_Header;

    return;
}

uint8_t ROS_CheckOpcode(uint8_t * byte){
    assert(byte);
    uint8_t rc = 0;

    switch(*byte){
        case ROS_Thurster:
            ROS.rx.id = ROS_Thurster;
            ROS.decoder.pkt_len = sizeof(Thruster_Pkt_t);
            break;
        case ROS_AHRS:
            ROS.rx.id = ROS_AHRS;
            ROS.decoder.pkt_len = sizeof(AHRS_Pkt_t);
            break;
        case ROS_Temperature:
            ROS.rx.id = ROS_Temperature;
            ROS.decoder.pkt_len = sizeof(Temp_Pkt_t);
            break;
        case ROS_Pressure:
            ROS.rx.id = ROS_Pressure;
            ROS.decoder.pkt_len = sizeof(Pressure_Pkt_t);
            break;
        default:
            ROS.rx.id = BAD_PKT;
            rc = 1;
            break;
    }

    return rc;
}
 /* End of Private Functions */
/// EOF
