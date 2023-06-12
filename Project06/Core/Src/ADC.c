/*
 * ADC.c
 *
 *  Created on: Nov 28, 2022
 *      Author: liams
 */

#include "ADC.h"
#include <string.h>
#include "math.h"

void get_frequency(ADC_info_t *ADC_info, uint16_t *ADC_buffer) {
	int i;
	int points = 0;
	uint8_t low = 0;
	uint16_t first_point_index, second_point_index;

	for (i = 0; i < ((1000.0/(float)ADC_info->read_period)*SAMPLES_PER_SECOND) && points < 2; i++) {
		// Get frequency for Sine and Triangle
		if (ADC_buffer[i] >= (ADC_info->maxv - MAX_VOLTAGE_BUFFER) && ADC_info->signal != Rectangle) {
			if (points%2 == 0) {
				first_point_index = i;
				points++;
			}
			else if (points%2 != 0) {
				if (i > first_point_index + POINT_DELAY) {
					second_point_index = i;
					ADC_info->samples = second_point_index - first_point_index;
					// Compute the frequency
					ADC_info->freq = (SAMPLES_PER_SECOND/ADC_info->samples);
					points++;
				}
			}
		}
		// Get frequency for Rectangle
		else if (ADC_info->signal == Rectangle)  {
			if (ADC_buffer[i] <= (ADC_info->minv + MAX_VOLTAGE_BUFFER)){
				low = 1;
			}
			if (ADC_buffer[i] == (ADC_info->maxv - MAX_VOLTAGE_BUFFER) && low == 1) {
				if (points == 0) {
					first_point_index = i;
					points++;
					low = 0;
				}
				else if(points == 1){
					second_point_index = i;
					points++;
					ADC_info->samples = second_point_index - first_point_index;
					// Compute the frequency
					ADC_info->freq = (SAMPLES_PER_SECOND/ADC_info->samples);
					break;
				}
			}
		}
	}
}

void get_signal_type(ADC_info_t *ADC_info, int *buckets, uint16_t *ADC_buffer) {
	int increment = 410;
	int i = ADC_info->minv / increment;
	int signal_points = 10000;
	int first = 0, second, third;
	int first_slope, second_slope;
	uint8_t found = 0;

//	// Check for Rectangle wave
	if (buckets[i] > (buckets[i+1] + buckets[i+2] + buckets[i+3])) {
		ADC_info->signal = Rectangle;
		return;
	}
	// Check for Sine and Triangle Wave
	else {
		// Check to see if the slop is linear (Triangle)
		for (i = 0; i < signal_points; i ++) {
			// Get first point for both slopes slopes
			if (ADC_buffer[i] == ADC_info->maxv && signal_points == 10000) {
				signal_points = i + (ADC_info->samples);
				first = i;
			}

			// Find next two points along that slope
			if (first != 0) {
				if (i >= first + (signal_points/3)) {
					second = i;
					first_slope = abs((int)(((ADC_info->maxv - ADC_buffer[i])/(second - first))));
				}
				else if (i >= first + (signal_points/2)) {
					third = i;
					second_slope = abs((int)((ADC_info->maxv - ADC_buffer[i])/(third - first)));
					found = 1;
				}
			}

			// If slopes match it means linear or triangle, if not then non-linear or sine
			if (second_slope != first_slope && found == 1) {
				if (second_slope != first_slope) { ADC_info->signal = Sine; break;}
				else { ADC_info->signal = Arbitrary; break;}
			}
			else if (found == 1) {
				ADC_info->signal = Triangle;
				break;
			}
		}

	}

	// Unknown Wave
	ADC_info->signal = Arbitrary;
}

void get_min_max(ADC_info_t *ADC_info, uint16_t *ADC_buffer) {
	int i;

	for(i = 0; i < ((1000.0/(float)ADC_info->read_period)*SAMPLES_PER_SECOND); i++) {
		// Get min and max
		if (ADC_buffer[i] > ADC_info->maxv) {ADC_info->maxv = ADC_buffer[i];}
		else if (ADC_buffer[i] < ADC_info->minv) {ADC_info->minv = ADC_buffer[i];}
	}
}

void get_buckets(uint16_t *ADC_buffer, int *buckets, ADC_info_t *ADC_info) {
	int i;

	for(i = 0; i < ((1000.0/(float)ADC_info->read_period)*SAMPLES_PER_SECOND); i++) {
		// Put results into Buckets
		if (ADC_buffer[i] < 411) { buckets[0]++; }
		else if (ADC_buffer[i]  < 821) {  buckets[1]++; }
		else if (ADC_buffer[i]  < 1230) {  buckets[2]++; }
		else if (ADC_buffer[i]  < 1640) {  buckets[3]++; }
		else if (ADC_buffer[i]  < 2050) {  buckets[4]++; }
		else if (ADC_buffer[i]  < 2460) {  buckets[5]++; }
		else if (ADC_buffer[i]  < 2870) {  buckets[6]++; }
		else if (ADC_buffer[i]  < 3280) {  buckets[7]++; }
		else if (ADC_buffer[i]  < 3690) {  buckets[8]++; }
		else if (ADC_buffer[i]  <= 4095) {  buckets[9]++; }
	}
}

void get_user_input(ADC_info_t *ADC_info) {
	char entered[50];
	int cnt = 0;

	// Get the user input
	get_line(entered, sizeof(entered));
	// See if the Capture was inputted
	if (entered[0] == 'C' || entered[0] == 'c') {ADC_info->mode = capture; return;}
	else {
		char * token = strtok(entered, " ");
		cnt++;
	    // loop through the string to extract all other tokens
	    while( token != NULL ) {
	    	switch(cnt) {
	    	case 1:
	    		ADC_info->mode = replay;
	    		break;
	    	case 2:
	    		break;
	    	case 3:
	    		ADC_info->replay_length = atoi(token);
	    	}
	    }
	    token = strtok(NULL, " ");
	    cnt++;
	}
}

void print_captured_info(ADC_info_t *ADC_info) {
	// Printing
	char str[50];

	print(USART2,"Captured Signal Information:\r\n");
	print(USART2, "Signal Type: ");
	switch(ADC_info->signal) {
		case Rectangle:
			print(USART2, "Rectangle\r\n");
			break;
		case Triangle:
			print(USART2, "Triangle\r\n");
			break;
		case Sine:
			print(USART2, "Sine\r\n");
			break;
		case Arbitrary:
			print(USART2, "Unknown\r\n");
			break;
	}
	sprintf(str, "Max Voltage: %d\r\n", (int)ADC_info->maxv);
	print(USART2, str);
	sprintf(str, "Min Voltage: %d\r\n", (int)ADC_info->minv);
	print(USART2, str);
	sprintf(str, "Frequency: %d\r\n", (int)ADC_info->freq);
	print(USART2, str);
	sprintf(str, "Number of samples in one period: %d\r\n", (int)ADC_info->samples);
	print(USART2, str);
}


void ADC_capture(ADC_info_t *ADC_info, uint16_t *ADC_buffer) {
	// ADC Variables
	uint32_t start_time = 0;
	uint8_t capture_performed = 0;

	// "Buckets" for the ADC readings
	int buckets[10] = {0,0,0,0,0,0,0,0,0,0};

	while(1) {

		/* *** Start/Stop collecting ADC Data *** */
		if (ADC_info->mode == capture) {
			// Get the start time
			start_time = TIM3->CNT;
			// Start capture
			HAL_ADC_Start_DMA(ADC_info->handle, (uint32_t *)ADC_buffer, ADC_BUFFER_SIZE);
			// Set mode to waiting
			capture_performed = 1;
			ADC_info->mode = waiting;
			// Print
			print(USART2, "Starting capture...\r\n");
		}
		// Wait the amount of time specified to capture
		else if (capture_performed == 1 && TIM5->CNT >= (start_time + (ADC_info->read_period*1000))) {
			HAL_ADC_Stop_DMA(ADC_info->handle);
			print(USART2, "Capture complete.\r\n");
			ADC_info->mode = analyze;
		}

		/* *** Analyze the Collected Data *** */
		if (ADC_info->mode == analyze) {
			// Get the results into buckets
			get_buckets(ADC_buffer, buckets, ADC_info);
			// Get min and max values
			get_min_max(ADC_info, ADC_buffer);
			// Get the frequency
			get_frequency(ADC_info, ADC_buffer);
			// Get the Signal Type
			get_signal_type(ADC_info, buckets, ADC_buffer);
			// Print out the data from the capture
			print_captured_info(ADC_info);
			// Change mode to waiting
			ADC_info->mode = waiting;
			// Set capture performed to 0
			capture_performed = 0;
			ADC_info->maxv = 0;
			ADC_info->minv = 4095;
			ADC_info->freq = 0;
			ADC_info->samples = 0;
		}

		/* *** Waiting on instruction *** */
		if (ADC_info->mode == waiting) {
			if (capture_performed == 0) {
				get_user_input(ADC_info);
			}
		}

		/* *** Replay Mode *** */
		if (ADC_info->mode == replay) {
			// Do nothing
		}

	}

}
