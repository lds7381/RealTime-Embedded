/*
 * ADC.h
 *
 *  Created on: Nov 28, 2022
 *      Author: liams
 */

#include "DAC.h"
#include "main.h"
#include "uart.h"
#include "math.h"
#include <stdlib.h>

#ifndef INC_ADC_H_
#define INC_ADC_H_

#define ADC_BUFFER_SIZE (32768)

#define POINT_DELAY (2)
#define MAX_VOLTAGE_BUFFER (15)

#define SAMPLES_PER_SECOND (10000)
#define _12_BIT_MASK (0b0000111111111111)

typedef enum {
	waiting,
	capture,
	analyze,
	replay
} ADC_mode_t;

typedef struct {
	ADC_HandleTypeDef *handle;
	uint16_t maxv;
	uint16_t minv;
	signal_type_t signal;
	uint32_t freq;
	uint32_t read_period; 	// in ms
	uint32_t samples;
	ADC_mode_t mode;
	uint16_t replay_length;
} ADC_info_t;

void ADC_capture(ADC_info_t *ADC_info, uint16_t *ADC_buffer);
void get_min_max(ADC_info_t *ADC_info, uint16_t *ADC_buffer);
void get_buckets(uint16_t *ADC_buffer, int *buckets, ADC_info_t *ADC_info);
void print_captured_info(ADC_info_t *ADC_info);
void get_frequency(ADC_info_t *ADC_info, uint16_t *ADC_buffer);
void get_user_input(ADC_info_t *ADC_info);
void get_signal_type(ADC_info_t *ADC_info, int *buckets, uint16_t *ADC_buffer);

#endif /* INC_ADC_H_ */
