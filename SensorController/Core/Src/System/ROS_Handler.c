/**
 * @file ROS_Handler.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "main.h"
#include "FreeRTOS.h"
#include "System/TaskCtrl.h"
#include "System/ROS_Handler.h"
#include "Peripherals/usart.h"


/* RTOS Task */
/**
 * @brief 
 * 
 */
void ROSHandler(void){
	
    DAT_USART_Handle_t *ROS_h = &uarts[ROS]; 
    uint8_t in;

	while(1){
        if(pdFAIL == xQueueReceive(ROS_h->queue_h,&in,portMAX_DELAY)) continue;

        // data has been received from the queue, now do something
        if(UART_Write(ROS_h,&in,1) != HAL_OK){
            HAL_GPIO_WritePin(LD1_GPIO_Port,LD1_Pin,GPIO_PIN_SET);
        }
    }
}