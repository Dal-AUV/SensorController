/**
 * @file watchdog.h
 * @author Wyatt Shaw
 * @brief files contains definitions for the watchdog timer implementations
 * * @version 0.1
 * @date 2023-09-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "main.h"
#include "FreeRTOS.h"

extern IWDG_HandleTypeDef hiwdg;

#ifndef WATCHDOG_H

/* Macros */
#define WATCHDOG_COUNTER 2499
/* Notes on this value
 * See equation RL from https://controllerstech.com/iwdg-and-wwdg-in-stm32/
 * RL =( (TIME_IN_Ms * 32000) / (4 * (2 ^ Pr) * 1000) ) - 1
 * Where Pr is the prescaler number (not value, ie prescaler of 4 would be 2^0, 8 would be 2^1
 * Additional Notes
 * https://www.st.com/resource/en/product_training/STM32WB-WDG_TIMERS-Independent-Watchdog-IWDG.pdf
 */

/* Prototypes */

void Watchdog_Init(void);
void MX_IWDG_Init(void);


#define WATCHDOG_H

#endif
