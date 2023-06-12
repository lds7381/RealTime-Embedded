/*
 * gamemaster.h
 *
 *  Created on: Oct 26, 2022
 *      Author: liams
 */

#ifndef INC_GAMEMASTER_H_
#define INC_GAMEMASTER_H_

#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "buttons.h"

typedef struct gamemaster_t {
	struct servo_profile_t *servo01;
	struct servo_profile_t *servo02;
	uint32_t total_time;
	uint32_t start_time;
	uint32_t end_time;
	GPIO_TypeDef *D4_SEG7_Latch_Port;
	uint16_t D4_SEG7_Latch_Pin;
	GPIO_TypeDef *D8_SEG7_Data_Port;
	uint16_t D8_SEG7_Data_Pin;
	GPIO_TypeDef *D7_SEG7_Clock_Port;
	uint16_t D7_SEG7_Clock_Pin;
} gamemaster_t;


void gamemaster_task(void *argument);
void seg_write_time(gamemaster_t *gamemaster);
void shiftOut(GPIO_TypeDef* data_port, uint16_t data_pin, GPIO_TypeDef* clock_port, uint16_t clock_pin, uint8_t value);
void clear_seg7(gamemaster_t *gamemaster) ;

#endif /* INC_GAMEMASTER_H_ */
