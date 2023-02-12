#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Peripherals/usart.h"
#include "System/TaskCtrl.h"

static void TASKHeartBeat(void);

uint8_t SysTask_Init(void){
	/* Added task to Kernel List */
	if(pdPASS != xTaskCreate(TASKDebugParser,"DebugParse",
			configMINIMAL_STACK_SIZE + 150,NULL, tskIDLE_PRIORITY +2,NULL)){
		return -1;
	}
	if(pdPASS != xTaskCreate(TASKHeartBeat,"HeartBeat",
			configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY +1, NULL)){
		return -1;
	}

	return 0;
}

static void TASKHeartBeat(void){

    uint8_t counter = 0;

    while(1){
        DebugWrite("HeartBeat: %X\n\r",counter++);
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        if(counter == 1000) counter = 0;

        vTaskDelay(1000);
    }
}
