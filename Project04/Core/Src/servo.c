/*
 * servo.c
 *
 *  Created on: Oct 26, 2022
 *      Author: Liam Sullivan
 */

#include "servo.h"

void lead_servo_task(void *argument) {

	// Get the servo profile from the argument
	struct servo_profile_t *servo = (servo_profile_t *)argument;

	// Get start time
	servo->start_time = TIM5->CNT;
	// Generate next random move time
	servo->next_wait = get_rand_num(RNG->DR, ONE_SECONDS, FOUR_SECONDS);
	servo->move_wait = get_move_wait(servo->last_position, servo->position);
	while (1) {
		// Wait for the servo to be in position
		if (TIM5->CNT >= servo->start_time + servo->move_wait && servo->rounds < MAX_ROUNDS) {
			// Check movement time (is it time to move to new position)
			if (TIM5->CNT >= (servo->start_time + servo->next_wait)) {
				// Save last position
				servo->last_position = servo->position;
				// Generate new position
				servo->position = (uint8_t)get_rand_num(RNG->DR, MIN_POSITION, MAX_POSITION);
				// Keep getting new position if the new one was the same as the last
				while(servo->position == servo->last_position) { servo->position = (uint8_t)get_rand_num(RNG->DR, MIN_POSITION, MAX_POSITION); }
				// Generate next random move time
				TIM2->CCR1 = get_duty_cycle_from_position(servo->position);
				// Get the start time
				servo->start_time = TIM5->CNT;
				// move servo to position
				servo->move_wait = get_move_wait(servo->last_position, servo->position);
				servo->next_wait = get_rand_num(RNG->DR, ONE_SECONDS, FOUR_SECONDS);
				// Change the status to moving
				servo->status = moving;
				// Increase the amount of round that have happened
				servo->rounds++;
			}
			// Change the moving status to in position
			else if (servo->status == moving) {
				servo->status = in_position;
			}
		}
		else if (servo->rounds >= MAX_ROUNDS) {
			if (TIM5->CNT >= (servo->start_time + servo->move_wait)) {
				servo->status = in_position;
			}
		}
	}
}

void user_servo_task(void *argument) {
	button_status b1;
	button_status b3;

	// Get the servo profile from the argument
	struct servo_profile_t *servo = (servo_profile_t *)argument;

	while (1) {
		// Left button press if the servo is in position
		if (TIM5->CNT >= (servo->start_time + servo->move_wait)) {
			// Set to in position
			if (servo->status == moving) { servo->status = in_position; }
			// Check Button 1 PRess
			b1 = check_button01();
			if (b1 == pressed) {
				servo->last_position = servo->position;
				if (servo->position < MAX_POSITION) {
					servo->position++;
				}
				servo->start_time = TIM5->CNT;
				TIM3->CCR3 = get_duty_cycle_from_position(servo->position);
				servo->move_wait = get_move_wait(servo->last_position, servo->position);
				servo->status = moving;
			}
			// Check Button 3 Press
			b3 = check_button03();
			if (b3 == pressed ) {
				servo->last_position = servo->position;
				if (servo->position > MIN_POSITION) {
					servo->position--;
				}
				servo->start_time = TIM5->CNT;
				TIM3->CCR3 = get_duty_cycle_from_position(servo->position);
				servo->move_wait = get_move_wait(servo->last_position, servo->position);
				servo->status = moving;
			}
		}
	}
}

void calibrate_servo() {
	button_status b1;
	button_status b2;
	button_status b3;
	uint32_t duty_cycle = DEFUALT_DUTY_CYCLE;
	uint32_t position_increment;
	uint8_t middle_pressed = 0;
	char info[200];

	TIM2->CCR1 = duty_cycle;

	// Position 0 Calibration
	print(USART2, "----- Calibrating Servos -----\r\nCalibration 1 : Position 0, Move fully right. Press Middle Button when done\r\n");
	while (middle_pressed != 2) {
		b3 = check_button03();
		if (b3 == pressed) {
			if (duty_cycle > MIN_DUTY_CYCLE) {
				duty_cycle -= CAL_DUTY_CYCLE_INCREMENT;
				TIM2->CCR1 = duty_cycle;
			}
		}
		b1 = check_button01();
		if (b1 == pressed) {
			if (duty_cycle < MAX_DUTY_CYCLE) {
				duty_cycle += CAL_DUTY_CYCLE_INCREMENT;
				TIM2->CCR1 = duty_cycle;
			}
		}
		b2 = check_button02();
		if (b2 == pressed) {
			if (middle_pressed == 0) {
				middle_pressed++;
				position0 = duty_cycle;
				duty_cycle = DEFUALT_DUTY_CYCLE;
				TIM2->CCR1 = duty_cycle;
				print(USART2, "Calibration 2 : Position 5, Move fully left. Press Middle Button when done\r\n");
			}
			else if (middle_pressed == 1){
				middle_pressed++;
				position5 = duty_cycle;
				duty_cycle = position0;
				TIM2->CCR1 = duty_cycle;
				TIM3->CCR3 = duty_cycle;

				position_increment = (position5 - position0) / 5;
				position1 = position0 + position_increment;
				position2 = position0 + (position_increment * 2);
				position3 = position0 + (position_increment * 3);
				position4 = position0 + (position_increment * 4);
				sprintf(info, "Position 0: %d, Position 1: %d, Position 2: %d, Position 3: %d, Position 4: %d,Position 5: %d\r\n----- Calibration Done -----\r\nPress the middle button to begin!\r\n", (int)position0, (int)position1, (int)position2, (int)position3, (int)position4, (int)position5);
				print(USART2, info);
			}
		}
	}
	while (check_button02() == not_pressed);
	print(USART2, "CATCH ME IF YOU CAN!!!\r\n");

}

uint32_t get_duty_cycle_from_position(uint8_t position) {
	switch (position) {
		case 0:
			return position0;
		case 1:
			return position1;
		case 2:
			return position2;
		case 3:
			return position3;
		case 4:
			return position4;
		case 5:
			return position5;
		default:
			return position0;
	}
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

