/*
 * random.h
 *
 *  Created on: Oct 17, 2022
 *      Author: liams
 */

#ifndef INC_RANDOM_H_
#define INC_RANDOM_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

float get_rand_num(uint32_t rand32, float min, float max);


#endif /* INC_RANDOM_H_ */
