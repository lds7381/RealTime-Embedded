/*
 * utils.c
 *
 *  Utility Class created for project #02 Real Time and Embedded Systems
 *
 *  Created on: Sep 28, 2022
 *      Author: Liam Sullivan
 */

#include "utils.h"

static uint8_t index = 0; // 0 = servo 0, 1 = servo 1

// Gets the commands inputted into the terminal
// Will add the commands to an array that contains the two servo instructions [servo0 command, servo1 command]
//
_Bool get_servo_process(servo_commands_t *entered, USART_TypeDef * USARTx) {
	char read;

	// Get what servo we are commanding
	if (entered[0] == none) {
		index = 0;
	} else if (entered[1] == none) {
		index = 1;
	}

	// Make sure a character has been gotten
	read = USART_Read(USARTx);
	if (read != '\0') {
		// Display inputed char to the terminal
		USART_Write(USARTx, &read, 1);
		switch (read) {
			case 'p':
				entered[index] = servo_pause;
				break;
			case 'c':
				entered[index] = servo_continue;
				break;
			case 'r':
				entered[index] = servo_right;
				break;
			case 'l':
				entered[index] = servo_left;
				break;
			case 'n':
				entered[index] = servo_no_op;
				break;
			case 'b':
				entered[index] = servo_begin;
				break;
			case 'x':
				// reset the entered commands
				entered[0] = none;
				entered[1] = none;
				break;
			case CR:
				USART_Write(USARTx, "\r\n Performing Commands... \r\n", 29);
				return TRUE;
				break;
			default:
				break;
		}
		if (index == 0) { index++; }
	}
	return FALSE;
}

uint32_t get_duty_cycle_from_position(uint8_t position) {
	switch (position) {
		case 0:
			return POSITION_0;
		case 1:
			return POSITION_1;
		case 2:
			return POSITION_2;
		case 3:
			return POSITION_3;
		case 4:
			return POSITION_4;
		case 5:
			return POSITION_5;
		default:
			break;
	}
	return POSITION_0;
}

uint32_t get_move_wait(uint8_t old_position, uint8_t new_position){
	uint8_t difference;
	uint32_t move_wait;

	if (old_position >= new_position) {
		difference = old_position - new_position;
	}
	else if (new_position > old_position) {
		difference = new_position - old_position;
	}
	move_wait = difference * POSITION_MOVE_WAIT;
	return move_wait;
}

void set_led( uint32_t on, GPIO_TypeDef *GPIO,  uint16_t pin){
	if ( on ) // a5
		HAL_GPIO_WritePin(GPIO, pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIO, pin, GPIO_PIN_SET);
}


