/*
 * customer.h
 *
 *  Created on: Oct 12, 2022
 *      Author: liams
 */

#ifndef INC_CUSTOMER_H_
#define INC_CUSTOMER_H_

#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "random.h"

#define MAX_MIN (8.0)
#define MIN_MIN (0.5)

#define ONE_MINUTE_TICKS  (1000)

typedef enum {
	customer_in_queue,
	customer_being_helped,
	customer_done
} customer_status_t;

typedef struct customer_t {
	/* customer information */
	uint32_t customer_id;
	uint32_t help_time;
	customer_status_t status;
	/* Stored Information */
	uint32_t time_spent_in_queue;
	uint32_t queue_start_time;
	uint32_t queue_end_time;
}	customer_t;

customer_t generate_customer();

#endif /* INC_CUSTOMER_H_ */
