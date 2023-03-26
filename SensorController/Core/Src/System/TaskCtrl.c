#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Peripherals/usart.h"
#include "Peripherals/i2c.h"
#include "System/TaskCtrl.h"

static void TASKHeartBeat(void);
void TASKI2C_Communication(void *pvParameters);
void TASKI2C_Write(void);

static const char *i2cMember = &hi2c1; //not sure how to pass this in

uint8_t SysTask_Init(void){
	/* Added task to Kernel List */
	if(pdPASS != xTaskCreate(TASKDebugParser,"DebugParse",
			configMINIMAL_STACK_SIZE + 150,NULL, tskIDLE_PRIORITY +3,NULL)){
		return -1;
	}
	if(pdPASS != xTaskCreate(TASKI2CDebugParser,"DebugParse",
			configMINIMAL_STACK_SIZE + 150,NULL, tskIDLE_PRIORITY +3,NULL)){
		return -1;
	}
	if(pdPASS != xTaskCreate(TASKHeartBeat,"HeartBeat",
			configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY +1, NULL)){
		return -1;
	}
	if(pdPASS != xTaskCreate(TASKI2C_Communication, "I2C_Communication",
			configMINIMAL_STACK_SIZE + 200, (void*) i2cMember, tskIDLE_PRIORITY+2, NULL)) { //will need some guidance on what kind of stack size and prio we need
		return -1;
	}
	if(pdPASS != xTaskCreate(TASKI2C_Write, "I2C_Write",
			configMINIMAL_STACK_SIZE + 200, (void*) i2cMember, tskIDLE_PRIORITY+2, NULL)) { //will need some guidance on what kind of stack size and prio we need
		return -1;
	}

	return 0;
}

static void TASKHeartBeat(void){

    uint8_t counter = 0;

    while(1){
        DebugWrite("HeartBeat: %X\n\r",counter++);
        //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

        if(counter == 1000) counter = 0;

        vTaskDelay(1000);
    }
}

void TASKI2C_Write(void) {

	uint8_t buf[100];

	if(xQueueReceive(xI2CQueue, &buf, NULL) != pdPASS) {
		//error
	}
	int i = 0;

	while(1) {
		i2cWrite("Arduino Value: %X", buf[i]);

		if(i == 100) {
			i = 0;
		}

		vTaskDelay(1000);
	}

}




void TASKI2C_Communication(void *pvParameters) {

	HAL_StatusTypeDef ret; //communication check variable
	uint8_t buf[12];
	uint16_t val;

	static const uint8_t ARD_ADDR = 0x7F << 1; //Address of Arduino
	//static const uint8_t OUTPUT_REG = 0x00 << 1; // Address of Register we want

	buf[0] = 0; //placeholder value
	BaseType_t xStatus = {0};



	while(1) {
		//communication for whether to read or write
		ret = HAL_I2C_Master_Transmit(&hi2c1, ARD_ADDR, buf, 1, HAL_MAX_DELAY);
		//this will trigger the call back, so maybe the following if's should be used elsewhere

		if(ret != HAL_OK) {
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		}
		else {
			//commununication to read the next two bytes from slave
			ret = HAL_I2C_Master_Receive(&hi2c1, ARD_ADDR, buf, 2, HAL_MAX_DELAY);
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			if(ret != HAL_OK) {
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
			}
			else {

				val = ((int16_t)buf[0]) << 4 | (buf[1] >> 4); //combining the bytes

				//sample implementation
				if(xQueueSendToBack(xI2CQueue, val, (TickType_t) 10) != pdPASS) {
				//don't turn the light off, still error
				}
				else {
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
				}
			}
		}
	}
}
