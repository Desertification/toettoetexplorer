/*
 * wait.c
 *
 *  Created on: 15-nov.-2017
 *      Author: robin
 */

#include "wait.h"

void wait_10us(uint16_t amount) {
	LPC_TIM1->TCR = 2; //reset TC and PR;
	LPC_TIM1->PR = 23; //TC should count microseconds
	LPC_TIM1->MR0 = (10 * amount) - 9; //Input functions give delay of +- 9 microseconds. This must be considered when working with small delays.
	LPC_TIM1->MCR = 6; //On match: Stop timer + reset.
	LPC_TIM1->TCR = 1; //enable counting
	while (LPC_TIM1->TCR == 1);
}

void wait_ms(uint16_t amount) {
	LPC_TIM1->TCR = 2; //reset TC and PR;
	LPC_TIM1->PR = 23; //TC should count microseconds
	LPC_TIM1->MR0 = 1000 * amount;
	LPC_TIM1->MCR = 6; //On match: Stop timer + reset.
	LPC_TIM1->TCR = 1; //enable counting
	while (LPC_TIM1->TCR == 1);
}

