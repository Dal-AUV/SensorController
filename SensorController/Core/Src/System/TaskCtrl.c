/**
 * @file TaskCtrl.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/* Headers */
#include <stdio.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Peripherals/usart.h"
#include "System/TaskCtrl.h"

static void TASKHeartBeat(void);

uint8_t SysTask_Init(void){
	/* Added task to Kernel List */
	 if(pdPASS != xTaskCreate(TASKHeartBeat,"HeartBeat",
	 		configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY +1, NULL)){
	 	return -1;
	 }
	return 0;
}

static void TASKHeartBeat(void){

    uint8_t counter = 0;
	uint8_t buffer[50];
	uint32_t timeout = 3000;

    while(1){

        sprintf(buffer,"HeartBeat: %02X\n\r",counter++);
        ROS_Write(buffer, 15, timeout);
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        if(counter == 1000) counter = 0;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
