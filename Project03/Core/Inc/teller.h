/*
 * teller.h
 *
 *  Created on: Oct 12, 2022
 *      Author: liams
 */

#ifndef INC_TELLER_H_
#define INC_TELLER_H_

#include <stdint.h>
#include <stdio.h>
#include "customer.h"
#include "uart.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "random.h"

#define MAX_WAIT (16000)

#define MAX_BREAK_TIME (1000.0)
#define MIN_BREAK_TIME (4000.0)

typedef enum {
	no_customer,
	helping_customer,
	on_break,
	force_break
} teller_status_t;

typedef struct teller_t {
	/* teller information */
	uint8_t  teller_id;
	customer_t current_customer;
	osMessageQueueId_t *queue_handle;
	osSemaphoreId_t *sem;
	teller_status_t status;
	uint32_t button_pin;
	GPIO_TypeDef *GPIO_port;
	uint32_t cust_start_time;
	uint8_t end_of_shift;
	uint8_t break_flag;
	uint8_t force_break_flag;
	uint32_t next_break_time;
	uint32_t break_time;
	uint32_t end_break_time;
	/* Stored information */
	uint32_t customers_helped;
	uint32_t total_help_time;
	uint32_t avg_customer_help_time;
	uint32_t avg_teller_wait_for_customer;
	uint32_t max_wait_for_customer;
	uint32_t start_wait_for_customer;
	uint32_t stop_wait_for_customer;
	uint32_t total_wait_time;
	uint32_t total_customer_queue_wait;
	uint32_t max_customer_help_time;
	uint32_t breaks_cnt;
	uint32_t avg_break_time;
	uint32_t longest_break_time;
	uint32_t shortest_break_time;
	uint32_t total_break_time;
	uint32_t max_cust_queue_wait;
	uint32_t start_force_break_time;
	uint32_t end_force_break_time;
} teller_t;


void teller_task(void *tell);
void init_teller(teller_t *teller);
void save_customer_information(customer_t *cust, teller_t *teller);

#endif /* INC_TELLER_H_ */
