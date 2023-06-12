/*
 * DAC.h
 *
 *  Created on: Nov 9, 2022
 *      Author: liams
 */

#include "main.h"
#include "uart.h"
#include "math.h"
#include <stdlib.h>

#ifndef INC_DAC_H_
#define INC_DAC_H_

#define WAVE_POINTS (256)
#define BASE_CLOCK_SPEED (80000000)

#define MAX_DAC_OUTPUT (4095)
#define MIN_DAC_OUTPUT (1)

#define MAX_V_OUTPUT (3.3)
#define MIN_V_OUTPUT (0)

#define PI (3.1415926)

typedef enum {
	Rectangle,
	Triangle,
	Sine,
	Arbitrary
} signal_type_t;

typedef struct {
	DAC_TypeDef *DAC_Handle;
	DAC_HandleTypeDef *hdac;
	uint8_t channel;
	signal_type_t signal;
	double frequency;
	uint32_t minv;
	uint32_t maxv;
	uint8_t noise;
	uint16_t noise_bits;
} DAC_info_t;


static uint32_t ekg[] = {
	1690, 1680, 1680, 1669, 1648, 1648, 1648, 1680, 1680, 1690, 1680, 1680, 1680, 1680, 1680, 1658,
	1690, 1690, 1712, 1690, 1690, 1680, 1669, 1669, 1680, 1690, 1690, 1680, 1669, 1669, 1669, 1680,
	1680, 1680, 1669, 1669, 1680, 1690, 1690, 1680, 1690, 1690, 1680, 1690, 1690, 1712, 1680, 1680,
	1658, 1648, 1648, 1648, 1669, 1669, 1680, 1690, 1690, 1701, 1680, 1680, 1669, 1680, 1680, 1680,
	1701, 1701, 1701, 1690, 1690, 1701, 1712, 1712, 1722, 1712, 1712, 1690, 1669, 1669, 1680, 1690,
	1690, 1690, 1733, 1733, 1765, 1776, 1861, 1882, 1936, 1936, 1968, 1989, 1989, 2032, 2053, 2053,
	2085, 2149, 2069, 2080, 2058, 2058, 1930, 1930, 1845, 1824, 1792, 1872, 1840, 1754, 1754, 1722,
	1680, 1680, 1680, 1637, 1637, 1637, 1637, 1637, 1626, 1648, 1648, 1637, 1605, 1605, 1616, 1680,
	1680, 1765, 1776, 1861, 2042, 2106, 2021, 1776, 2480, 2400, 2176, 1632, 1637, 1360, 933, 928,
	1962, 1962, 2042, 2149, 3141, 3141, 2320, 1200, 1200, 1392, 1669, 1669, 1658, 1701, 1701, 1701,
	1701, 1701, 1722, 1690, 1690, 1690, 1680, 1680, 1690, 1690, 1690, 1669, 1669, 1669, 1701, 1733,
	1733, 1754, 1744, 1744, 1733, 1733, 1733, 1722, 1765, 1765, 1765, 1733, 1733, 1733, 1722, 1722,
	1701, 1690, 1690, 1701, 1690, 1690, 1701, 1701, 1701, 1701, 1722, 1722, 1712, 1722, 1722, 1733,
	1733, 1733, 1733, 1712, 1712, 1712, 1733, 1733, 1733, 1733, 1733, 1733, 1744, 1744, 1744, 1744,
	1744, 1744, 1733, 1733, 1722, 1722, 1722, 1722, 1722, 1722, 1733, 1722, 1722, 1722, 1722, 1722,
	1701, 1669, 1669, 1680, 1690, 1690, 1690, 1701, 1701, 1712, 1712, 1712, 1690, 1669, 1669, 1680,
};



void get_DAC_info(DAC_info_t *DAC_info);
void start_DAC_signal(DAC_info_t *DAC_info, uint32_t *wave_points);
void get_rectangle_wave(DAC_info_t *DAC_info, uint32_t *wave_points);
void get_sine_wave(DAC_info_t *DAC_info, uint32_t *wave_points);
void get_triangle_wave(DAC_info_t *DAC_info, uint32_t *wave_points);
void get_arbitrary_wave(DAC_info_t *DAC_info, uint32_t *wave_points);
void scale_wave_points(DAC_info_t *DAC_info, uint32_t *wave_points);
uint32_t convert_analogV_digitalV(double voltage);

#endif /* INC_DAC_H_ */
