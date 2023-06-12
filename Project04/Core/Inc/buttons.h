/*
 * buttons.h
 *
 *  Created on: Nov 1, 2022
 *      Author: liams
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "stm32l4xx_hal.h"
#include "uart.h"

typedef enum {
	pressed,
	not_pressed
} button_status;

button_status check_button01();
button_status check_button02();
button_status check_button03();

#endif /* INC_BUTTONS_H_ */
