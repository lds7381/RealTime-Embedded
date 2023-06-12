/*
 * random.c
 *
 *  Created on: Oct 17, 2022
 *      Author: liams
 */

#include "random.h"

// Gets a random number between a given range with the random 32 bit number gotten
float get_rand_num(uint32_t rand32, float min, float max) {
	return (float) rand32 / 0xffffffff * (max - min) + min;
}


uint32_t convert_ticks_ms(uint32_t ticks) {
	return ticks / 1000;
}
