/*
 * teller.c
 *
 *  Created on: Oct 12, 2022
 *      Author: liams
 *
 * Class used to handle all teller information and tasks
 */

#include "teller.h"

static uint8_t teller_cnt = 0;

/* The main teller task for the three tellers
 * - Task will be run using FreeRTOS
 */
void teller_task(void *tell) {
	uint8_t working = 1;
	char str[50];
	osStatus_t stat;
	// Set the teller from the parameters of the task
	teller_t *teller = (teller_t *)tell;
	// Get the semaphore pointer
	osSemaphoreId_t *sem = teller->sem;
	// Aquire the semaphore
	osSemaphoreAcquire(*sem, 0);
	// Initalize the teller
	init_teller(teller);
	while(working) {
		switch(teller->status) {
			case helping_customer:
				// if the help time plus the start time is greater than the TIM2 cnt, the customer is still being helped
				if ((teller->cust_start_time + teller->current_customer.help_time) > TIM2->CNT) {
					// keep waiting until fully serviced (do nothing)
					if ((teller->cust_start_time + teller->current_customer.help_time) < TIM2->CNT + 20) {
						vTaskDelay(20);
					}
				}
				// TIM2 is less than help time + start time (customer has been helped)
				else {
					// Save customer information
					save_customer_information(&teller->current_customer, teller);
					// Start wait time for new customer
					teller->start_wait_for_customer = TIM2->CNT;
					if (teller->break_flag != 1 && teller->force_break_flag != 1) {
						// finish performing
						teller->status = no_customer;
					}
					else if (teller->break_flag == 1){
						// go on break
						teller->status = on_break;
					}
					else if (teller->force_break_flag == 1) {
						teller->status = force_break;
					}
				}
				break;
			case no_customer:
				// Get new customer from the queue
				stat = osMessageQueueGet(*(teller->queue_handle), &(teller->current_customer), NULL, 0);
				if (stat == osOK) {
					// Stop wait for new customer
					teller->stop_wait_for_customer = TIM2->CNT;
					// set the end queue time
					teller->current_customer.queue_end_time = teller->stop_wait_for_customer;
					// update total customer wait in queue time
					teller->total_customer_queue_wait += (teller->current_customer.queue_end_time - teller->current_customer.queue_start_time);
					// Update time gotten customer
					teller->cust_start_time = TIM2->CNT;
					// Set the status of teller to helping to perform
					teller->status = helping_customer;
				}
				else if (stat == osErrorResource && teller->end_of_shift == 1) {
					working = 0;
				}
				break;
			case on_break:
				// Generate a random break length for the teller 30-60mins plus the current time
				if (teller->break_time == 0) {
					teller->break_time = get_rand_num(RNG->DR, MIN_BREAK_TIME, MAX_BREAK_TIME);
					teller->end_break_time = (teller->break_time + TIM2->CNT);
					// update metrics
					if (teller->break_time > teller->longest_break_time) teller->longest_break_time = teller->break_time;
					if (teller->break_time < teller->shortest_break_time) teller->shortest_break_time = teller->break_time;
					teller->total_break_time += teller->break_time;
				}
				// wait until break is done
				if (TIM2->CNT >= teller->end_break_time) {
					// Change status to off break with no customer
					teller->status = no_customer;
					// update the break flag
					teller->break_flag = 0;
					// update the next break time
					teller->next_break_time = 0;
					// update the break time
					teller->break_time = 0;
					// Update the break count
					teller->breaks_cnt++;
				}
				break;
			case force_break:
				if (teller->force_break_flag == 1) {
					// do nothing
				}
				else {
					teller->end_force_break_time = TIM2->CNT;
					uint32_t break_time = (teller->end_force_break_time - teller->start_force_break_time);
					// update metrics
					if (break_time > teller->longest_break_time) teller->longest_break_time = break_time;
					if (break_time < teller->shortest_break_time) teller->shortest_break_time = break_time;
					teller->total_break_time += break_time;
					teller->breaks_cnt++;
					teller->status = no_customer;
				}
				break;
		}
	}
	osSemaphoreRelease(*sem);
	vTaskDelete( NULL );
}

// Saves custmer's relevant information to the teller
void save_customer_information(customer_t *cust, teller_t *teller) {
	// Increase customers helped
	teller->customers_helped += 1;
	// Update average customer help times
	teller->total_help_time += cust->help_time;
	teller->avg_customer_help_time = teller->total_help_time / teller->customers_helped;
	// Update average wait time
	teller->total_wait_time += (teller->stop_wait_for_customer - teller->start_wait_for_customer);
	teller->avg_teller_wait_for_customer = teller->total_wait_time / teller->customers_helped;
	// Save max customer wait
	if ((teller->stop_wait_for_customer - teller->start_wait_for_customer) > teller->max_wait_for_customer) teller->max_wait_for_customer = (teller->stop_wait_for_customer - teller->start_wait_for_customer);
	// Set max customer queue wait
	if ((cust->queue_end_time - cust->queue_start_time) > teller->max_cust_queue_wait) teller->max_cust_queue_wait = cust->queue_end_time - cust->queue_start_time;
	// Update max transaction time
	if (cust->help_time > teller->max_customer_help_time) teller->max_customer_help_time = cust->help_time;
}

// Intializes the teller to have its default values
void init_teller(teller_t *teller) {
	// Initialize the teller information
	teller->teller_id = teller_cnt;
	teller_cnt++;
	teller->break_time = 0;
	teller->break_flag = 0;
	teller->force_break_flag = 0;
	teller->next_break_time = 0;
	teller->status = no_customer;
	teller->cust_start_time = 0;
	teller->end_of_shift = 0;
	// Initialize the stored information for the teller
	teller->customers_helped = 0;
	teller->total_help_time = 0;
	teller->avg_customer_help_time = 0;
	teller->avg_teller_wait_for_customer = 0;
	teller->max_wait_for_customer = 0;
	teller->start_wait_for_customer = TIM2->CNT;
	teller->stop_wait_for_customer = 0;
	teller->total_wait_time = 0;
	teller->max_customer_help_time = 0;
	teller->breaks_cnt = 0;
	teller->avg_break_time = 0;
	teller->longest_break_time = 0;
	teller->shortest_break_time = 0xFFFFFFFF;
	teller->max_cust_queue_wait = 0;
	teller->total_break_time = 0;
	teller->total_customer_queue_wait = 0;
	teller->start_force_break_time = 0;
	teller->end_force_break_time = 0;
}
