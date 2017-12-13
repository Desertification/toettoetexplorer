/*
 * distance_sensor.c
 *
 *  Created on: 25-okt.-2017
 *      Author: robin
 */

#include "distance_sensor.h"

//states
const uint8_t SEND_PULSE = 0;
const uint8_t WAITING_FOR_ECHO = 1;
const uint8_t RECEIVE_ECHO = 2;

//pin names
const uint8_t TRIGPIN = 21;
const uint8_t ECHOPIN = 10;

static uint8_t measurement_state;
static uint16_t distance_in_cm;
static uint32_t duration_in_ms;


//static void send_pulse();
//static void wait_for_echo();
//static void receive_echo();
static void calculate_distance_out_of_duration(uint32_t duration_in_ms);

void distance_sensor_init() {
	gpio_init(TRIGPIN);
	gpio_init(ECHOPIN);
	gpio_set_direction(ECHOPIN, GPIO_PINDIRECTION_INPUT);
	gpio_set_mode(ECHOPIN, GPIO_PINMODE_NO_PULL);
	LPC_TIM0->PR = 23;//TC should count microseconds
	gpio_enable_interrupt_rising_edge(ECHOPIN);//enable interrupt to act when sending pulse
	gpio_enable_interrupt_falling_edge(ECHOPIN);//enable interrupt to act when receiving pulse
}

void distance_sensor_update_distance_cm() {
	send_pulse();
	wait_for_echo();
	receive_echo();
}

uint16_t distance_sensor_get_distance_cm() {
	return distance_in_cm;
}

void send_pulse() {
	measurement_state = SEND_PULSE;
	gpio_set(TRIGPIN);
	LPC_TIM0->MR0 = 10; //aiming for 10 us (13 now to be sure)
	LPC_TIM0->MCR = 3;//enable Interrupt + TC reset on match
	LPC_TIM0->TCR = 1;//Enable counter + Disable reset
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void wait_for_echo() {
	LPC_TIM0->MCR = 0;//Disable interrupt + TC reset
	measurement_state = WAITING_FOR_ECHO;
	NVIC_EnableIRQ(EINT3_IRQn);
}

void receive_echo() {
	measurement_state = RECEIVE_ECHO;
	NVIC_EnableIRQ(EINT3_IRQn);
}

void TIMER0_IRQHandler(void) {
	switch (measurement_state) {

	case 0 :
		LPC_TIM0->IR = 1; //clear the interrupt flag
		LPC_TIM0->TCR |= (1 << 1); //reset and keep timer at 0
		gpio_clear(TRIGPIN);
		break;

	}
}

void EINT3_IRQHandler(void) {
	LPC_SC->EXTINT = 8; //clear EINT3 interrupt flag
	switch (measurement_state) {

	case 1 :
		LPC_TIM0->TCR &= ~(1 << 1); //start counting from again by setting reset low.
		break;

	case 2 :
		duration_in_ms = LPC_TIM0->TC;
		calculate_distance_out_of_duration( duration_in_ms );
		LPC_TIM0->TCR |= (1 << 1);  //reset and keep timer at 0
		break;

	}
}

void calculate_distance_out_of_duration(uint32_t duration_in_ms) {
	const uint16_t WAVE_SPEED = 34; // centimeter/millisecond
	distance_in_cm = WAVE_SPEED * (duration_in_ms / 2);
}
