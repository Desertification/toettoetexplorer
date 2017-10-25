/*
 * distance_sensor.c
 *
 *  Created on: 25-okt.-2017
 *      Author: robin
 */

#include "distance_sensor.h"

static uint8_t measure_state;

void distance_sensor_init() {
	measure_state = 0;
	gpio_init(TRIGPIN);
	gpio_init(ECHOPIN);
	gpio_set_direction(ECHOPIN, GPIO_PINDIRECTION_INPUT);
	LPC_TIM0->PR = 23;//TC should count microseconds
	gpio_enable_interrupt_rising_edge(ECHOPIN);//enable interrupt to act when receiving pulse
}

uint16_t get_distance_cm() {
	return distance;
}

void send_pulse() {
	LPC_TIM0->MR0 = 10;
	LPC_TIM0->MCR = 3;//enable Interrupt + reset TC
	LPC_TIM0->TCR = 1;
	NVIC_EnableIRQ(TIMER0_IRQn);
	gpio_set(TRIGPIN);
}

void receive_echo() {
	LPC_TIM0->MCR = 0;//Disable interrupt and TC reset
	VIC_EnableIRQ(EINT3_IRQn);
}

void TIMER0_IRQHandler(void) {

	switch(measure_state) {

		case 0 :
			LPC_TIM0->IR = 1;
			gpio_clear(TRIGPIN);
	}
}

void EINT3_IRQHandler(void) {

	switch(measure_state) {

		case 0 :
			LPC_TIM0->IR = 1;
			gpio_clear(TRIGPIN);
	}
}
