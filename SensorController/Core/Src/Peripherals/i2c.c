/*
 * i2c.c
 *
 *  Created on: Feb. 13, 2023
 *      Author: wyatt
 */


/* Standard Headers */
#include "main.h"
#include <stdarg.h>
/* Middleware Headers */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"


/* Definitions */

#define MAX_I2C_BUF_SIZE 50

static const uint8_t ARD_ADDR = 0x7F << 1; //Address of Arduino




/* Globals */
QueueHandle_t xI2CQueue, xQueue2;

SemaphoreHandle_t TestMutex;

uint8_t TempBuff[MAX_I2C_BUF_SIZE];

/* Application Headers */
#include "Peripherals/i2c.h"
#include "Peripherals/usart.h"


//function to call for read/write
void i2c_Communication_Function(I2C_HandleTypeDef *hi2c) {

	HAL_StatusTypeDef ret; //communication check
	uint8_t buf[12];
	uint16_t val;

	buf[0] = 0;

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
			i2cWrite(val);
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
			//whatever we want with the 2 bytes from arduino
			//call uart or RTOS stuff
		}

	}

	return;

}

void i2cWrite(const char * format, ...) {

    char buffer[50];
    va_list args;
    // Get Format and parse it
    va_start(args, format);
    vsnprintf(buffer,sizeof(buffer),format, args);
    va_end(args);

    xSemaphoreTake(TestMutex, portMAX_DELAY);
    // Transmit via Debug USART
    HAL_UART_Transmit(&huart3,(uint8_t*)buffer,
        strlen(buffer), HAL_MAX_DELAY);
    // Give the lock
    xSemaphoreGive(TestMutex);

	return;
}

//when this happens I want to call the main i2c function to allow for writing? or do i
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {

	BaseType_t xStatus = {0};
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

	if (hi2c == &hi2c1) {
		xStatus = xQueueSendToBackFromISR(xI2CQueue, TempBuff, NULL);

	}
	if(xStatus == pdPASS){
	        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	}

	return;
}

void Request_I2CDebug_Read(void){

	HAL_I2C_Master_Receive_IT(&hi2c1,  ARD_ADDR,  TempBuff, 1); //will need to change the address location to abstract later...

    return;

}


void TASKI2CDebugParser(void){
    BaseType_t xStatus = {0};
    uint8_t in;
    uint8_t pos = 0;
    uint8_t buffer[50];
    while(1){
        xStatus = xQueueReceive(xI2CQueue,&in,portMAX_DELAY);
        if(xStatus == pdFALSE) continue;
        buffer[pos++] = in;
        if((in == '\n') | (in == '\r')){
            pos = 0;
            i2cWrite("%s\n",buffer);
            memset(&buffer,0,50);
        }else continue;
    }
    return;
}




















