/*
 * utils.h
 *
 *  Created on: Sep 28, 2022
 *      Author: liams
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "utils.h"
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

#define FALSE (0)
#define TRUE (1)

// Servo Position Duty Cycles
#define POSITION_0 (470)
#define POSITION_1 (776)
#define POSITION_2 (1082)
#define POSITION_3 (1388)
#define POSITION_4 (1694)
#define POSITION_5 (2000)

#define POSITION_MOVE_WAIT (2000)

// Defined commands that are valid
#define MOV (0x20)
#define WAIT (0x40)
#define LOOP (0x80)
#define END_LOOP (0xA0)
#define RECIPE_END (0)
#define SWEEP (0x60)

#define CR (13)

#define TIME_100MS (1000)

// ENUMS

// Status of the display.
enum status
{
	status_running,
	status_paused,
	status_command_error,
	status_nested_error
} ;

typedef enum
{
	servo_pause,
	servo_continue,
	servo_right,
	servo_left,
	servo_no_op,
	servo_begin,
	none
} servo_commands_t;


// STRUCTS

typedef struct servo_profile_t {
	unsigned char *recipe;
	servo_commands_t command;
	uint8_t position;
	uint8_t recipe_index;
	enum status status;
	uint8_t wait_cnt;
	uint8_t loop_start_index;
	uint8_t loop_amt;
	uint8_t loop_cnt;
	uint32_t comp_time;
	uint32_t move_wait;
	uint8_t sweep_cnt;
	uint8_t nest_prot;
} servo_profile_t;

// VARS


// FUNCTIONS

_Bool get_servo_process(servo_commands_t *entered, USART_TypeDef * USARTx);
uint32_t get_duty_cycle_from_position(uint8_t position);
uint32_t get_move_wait(uint8_t old_position, uint8_t new_position);
void update_leds(enum status status);
void set_led( uint32_t on, GPIO_TypeDef *GPIO,  uint16_t pin);


#endif /* INC_UTILS_H_ */
