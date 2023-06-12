/*
 * queue.h
 *
 *  Created on: Oct 12, 2022
 *      Author: liams
 */

#ifndef INC_MANAGER_H_
#define INC_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include "customer.h"
#include "uart.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os.h"
#include "teller.h"
#include "main.h"

#define MAX_QUEUE (20)

#define MIN_NEXT_BREAK_TIME (30000.0)
#define MAX_NEXT_BREAK_TIME (60000.0)

#define MIN_CUST_TIME (1000.0)
#define MAX_CUST_TIME (4000.0)

#define SEVEN_HOURS (420000) // should be 420000

#define SEVEN_SEG_ZERO (0b10000001)
#define SEVEN_SEG_ONE (0b10011111)
#define SEVEN_SEG_TWO (0b11001000)
#define SEVEN_SEG_THREE (0b11100001)
#define SEVEN_SEG_FOUR (0b10011001)
#define SEVEN_SEG_FIVE (0b01001001)

typedef struct manager_info_t {
	osThreadId_t *queue_handle;
	teller_t *teller01;
	teller_t *teller02;
	teller_t *teller03;
	int *idle_hook_cnt;
} manger_info_t;

void manager_task(void *manager_info);
void init_queue(uint32_t *size);
void check_force_break(teller_t *t);
void print_results(teller_t *t1, teller_t *t2, teller_t *t3);
void monitor(osMessageQueueId_t *q, teller_t *t1, teller_t *t2, teller_t *t3);
uint8_t get_display_from_size(uint32_t size);

#endif /* INC_MANAGER_H_ */
