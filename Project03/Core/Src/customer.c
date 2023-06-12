/*
 * customer.c
 *
 *  Created on: Oct 12, 2022
 *      Author: liams
 */

#include "customer.h"

static uint32_t customer_cnt = 0;

/* Generate Customer with Random values needed for the teller
 * - Creates a new customer and returns it initalized with random values
 */
customer_t generate_customer(){
	customer_t new_cust;
	// Set the customer id
	new_cust.customer_id = customer_cnt;
	customer_cnt++;
	// set the customer help time to a random number 30 seconds to 8 minutes
	new_cust.help_time = (get_rand_num(RNG->DR, MIN_MIN, MAX_MIN) * ONE_MINUTE_TICKS);
	// Set the status to in queue
	new_cust.status = customer_in_queue;
	// Set time it was generated
	new_cust.queue_start_time = TIM2->CNT;
	return new_cust;
}
