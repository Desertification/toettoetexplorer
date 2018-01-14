/*
 * distance_sensor.c
 *
 *  Created on: 25-okt.-2017
 *      Author: robin
 */

#include "distance_sensor.h"
#include "wait.h"

//states (used in interrupt handler)
const uint8_t SEND_PULSE = 0;
const uint8_t WAITING_FOR_ECHO = 1;
const uint8_t RECEIVE_ECHO = 2;

//pin names
const uint8_t TRIGPIN = 21;
const uint8_t ECHOPIN = 10;

static uint8_t measurement_state;//keeps track of current state

static uint16_t distance_in_cm;

static uint32_t duration_in_ms;//time need for beam to reflect back

//forward declarations
static void send_pulse();
static void wait_for_echo();
static void receive_echo();
static void calculate_distance_out_of_duration(uint32_t duration_in_ms);
static void update_distance_cm();

/**
 * Initialises the distance sensor
 */
void distance_sensor_init() {
	//making sure interrupt priorities are set right.
	NVIC_SetPriority(TIMER0_IRQn, TIMER0_IRQn);
	NVIC_SetPriority(EINT3_IRQn, EINT3_IRQn);

	//initialize pins
	gpio_init(TRIGPIN);
	gpio_init(ECHOPIN);

	//set input/output directions and/or pin modes
	gpio_set_direction(ECHOPIN, GPIO_PINDIRECTION_INPUT);
	gpio_set_mode(ECHOPIN, GPIO_PINMODE_NO_PULL);

	LPC_TIM0->PR = 23;//TC should count microseconds

	/* echo pin is set high when pulse is send and cleared when
	 * echo is received.
	 */
	gpio_enable_interrupt_rising_edge(ECHOPIN);//enable interrupt to act when sending pulse
	gpio_enable_interrupt_falling_edge(ECHOPIN);//enable interrupt to act when receiving pulse
}

/**
 * updates and return the current distance measured by the sensor
 *
 * @return current distance (cm)
 */
uint16_t distance_sensor_get_distance_cm() {
	update_distance_cm();
	return distance_in_cm;
}

/**
 * sets trig pin high for 10 us (pulse)
 * initialises timer 0 to count 10 us and send an interrupt
 */
void send_pulse() {
	measurement_state = SEND_PULSE;
	gpio_set(TRIGPIN);
	LPC_TIM0->MR0 = 10; //aiming for 10 us (13 now to be sure)
	LPC_TIM0->MCR = 3;//enable Interrupt + TC reset on match
	LPC_TIM0->TCR = 1;//Enable counter + Disable reset
	NVIC_EnableIRQ(TIMER0_IRQn);
}

/**
 * deactivate timer 0
 * sensor puts echo pin high when ready to receive echo
 * wait for interrupt on rising edge to start measuring travel time
 */
void wait_for_echo() {
	LPC_TIM0->MCR = 0;//Disable interrupt + TC reset
	measurement_state = WAITING_FOR_ECHO;
	NVIC_EnableIRQ(EINT3_IRQn);
}

/**
 * echo pin is set low when echo is received
 * wait for interrupt on falling edge to calculate distance.
 */
void receive_echo() {
	measurement_state = RECEIVE_ECHO;
	NVIC_EnableIRQ(EINT3_IRQn);
}

/**
 * interrupt handler for timer 0
 */
void TIMER0_IRQHandler(void) {
	switch (measurement_state) {

	//state = sending pulse
	case 0 :
		LPC_TIM0->IR = 1; //clear the interrupt flag
		LPC_TIM0->TCR |= (1 << 1); //reset and keep timer at 0
		gpio_clear(TRIGPIN);
		break;

	}
}

/**
 * interrupt handler for GPIO interrupts
 */
void EINT3_IRQHandler(void) {
	LPC_SC->EXTINT |= (1 << 3); //clear EINT3 interrupt flag
	switch (measurement_state) {

	//waiting for echo
	//by setting low reset timer will start counting from 0 and
	//time can be measured when echo is received
	case 1 :
		LPC_TIM0->TCR &= ~(1 << 1); //start counting from 0 again by setting reset low.
		break;

	//echo received
	//calculate distance out of wave travel time
	case 2 :
		duration_in_ms = LPC_TIM0->TC;
		calculate_distance_out_of_duration( duration_in_ms );
		LPC_TIM0->TCR |= (1 << 1);  //reset and keep timer at 0
		break;
	}
}

/**
 * calculate distance out of wave travel time
 * store the value in the global variable
 *
 * @param duration_in_ms: beam travel time
 */
void calculate_distance_out_of_duration(uint32_t duration_in_ms) {
	const uint16_t WAVE_SPEED = 34; // centimeter/millisecond
	distance_in_cm = WAVE_SPEED * (duration_in_ms / 2);
}

/**
 * cycle through all states to get update the distance value
 */
void update_distance_cm() {
	send_pulse();
	wait_for_echo();
	receive_echo();
}
