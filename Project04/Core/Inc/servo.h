/*
 * servo.h
 *
 *  Created on: Oct 26, 2022
 *      Author: Liam Sullivan
 */

#include "stm32l4xx_hal.h"
#include "random.h"
#include "cmsis_os.h"
#include "uart.h"
#include "buttons.h"

#define MIN_DUTY_CYCLE (400)
#define MAX_DUTY_CYCLE (2400)
#define DEFUALT_DUTY_CYCLE (1200)
#define CAL_DUTY_CYCLE_INCREMENT (50);

#define ONE_SECONDS (1000000)
#define FOUR_SECONDS (4000000)

#define MAX_POSITION (5)
#define MIN_POSITION (0)

#define MAX_ROUNDS (5)

#define POSITION_MOVE_WAIT (200000)

typedef enum {
	moving,
	in_position,
	game_complete
} status;

typedef struct servo_profile_t{
	uint8_t position;
	uint8_t last_position;
	status status;
	uint32_t start_time;
	uint32_t move_wait;
	uint32_t next_wait;
	uint32_t rounds;
} servo_profile_t;

static uint32_t position0;
static uint32_t position1;
static uint32_t position2;
static uint32_t position3;
static uint32_t position4;
static uint32_t position5;

void lead_servo_task(void *argument);
void user_servo_task(void *argument);
void calibrate_servo();
uint32_t get_move_wait(uint8_t old_position, uint8_t new_position);
uint32_t get_duty_cycle_from_position(uint8_t position);
