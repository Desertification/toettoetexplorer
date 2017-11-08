/*
 * distance_sensor.c
 *
 *  Created on: 25-okt.-2017
 *      Author: robin
 */

#include "distance_sensor.h"


//pinnames
const uint8_t TRIGPIN = 21;
const uint8_t ECHOPIN = 10;

static uint16_t distance_in_cm;

static uint32_t duration_in_ms;


static void send_pulse();
static void receive_echo();
static void calculate_distance_out_of_duration(uint32_t duration_in_ms);

void distance_sensor_init() {
	gpio_init(TRIGPIN);
	gpio_init(ECHOPIN);
	gpio_set_direction(ECHOPIN, GPIO_PINDIRECTION_INPUT);
	LPC_TIM0->PR = 23;//TC should count microseconds
	gpio_enable_interrupt_rising_edge(ECHOPIN);//enable interrupt to act when receiving pulse
}

void distance_sensor_update_distance_cm() {
	send_pulse();
	receive_echo();
}

uint16_t distance_sensor_get_distance_cm() {
	return distance_in_cm;
}

void send_pulse() {
	LPC_TIM0->MR0 = 10;
	LPC_TIM0->MCR = 3;//enable Interrupt + reset TC
	LPC_TIM0->TCR = 1;//Enable counter + Disable reset
	NVIC_EnableIRQ(TIMER0_IRQn);
	gpio_set(TRIGPIN);
}

void receive_echo() {
	LPC_TIM0->MCR = 0;//Disable interrupt and TC reset
	NVIC_EnableIRQ(EINT3_IRQn);
}

void TIMER0_IRQHandler(void) {
	LPC_TIM0->IR = 1;
	gpio_clear(TRIGPIN);
}

void EINT3_IRQHandler(void) {
	LPC_SC->EXTINT = 8; //clear EINT3 interrupt flag
	duration_in_ms = LPC_TIM0->TC;
	calculate_distance_out_of_duration( duration_in_ms );
	LPC_TIM0->TCR = 2; //Disable counter. Keep at reset value
}

void calculate_distance_out_of_duration(uint32_t duration_in_ms) {
	const uint16_t WAVE_SPEED = 34; // centimeter/millisecond
	distance_in_cm = WAVE_SPEED * (duration_in_ms / 2);
}
