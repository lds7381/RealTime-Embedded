/*
 * buttons.c
 *
 *  Created on: Nov 1, 2022
 *      Author: liams
 */

#include "buttons.h"

button_status check_button01() {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==GPIO_PIN_RESET) {
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)==GPIO_PIN_RESET);
		return pressed;
	}
	else {
		return not_pressed;
	}
}

button_status check_button02() {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)==GPIO_PIN_RESET) {
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4)==GPIO_PIN_RESET);
		return pressed;
	}
	else {
		return not_pressed;
	}
}

button_status check_button03() {
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)==GPIO_PIN_RESET) {
		while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)==GPIO_PIN_RESET);
		return pressed;
	}
	else {
		return not_pressed;
	}
}


