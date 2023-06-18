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
	
	while(1){
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}