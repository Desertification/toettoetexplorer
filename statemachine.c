/*
 * statemachine.c
 *
 *  Created on: Dec 19, 2017
 *      Author: thoma
 */

#include "statemachine.h"
#include "init.h"
#include "motor.h"
#include "uart.h"
#include "buzzer.h"
#include "wait.h"
#include "servo.h"
#include "distance_sensor.h"
#include "serial.h"
#include "display.h"


static state_t next_state;

//====================== forward declarations ======================//

static void init();
static void drive();
static void look();
static void turn_left();
static void turn_right();
static void stuck();

//====================== global functions ==========================//

/**
 * Runs the statemachine. is blocking.
 *
 * @param initial_state initial state to start in
 */
void statemachine_run(state_t initial_state){
	next_state = initial_state;
	while(1){
		switch (next_state) {
			case INIT:
				init();
				break;
			case DRIVE:
					drive();
					break;
			case LOOK:
					look();
					break;
			case STUCK:
					stuck();
					break;
			case TLEFT:
					turn_left();
					break;
			case TRIGHT:
					turn_right();
					break;
			default:
				break;
		}
	}
}

/**
 * Sets the next state of the statemachine.
 *
 * @param state next state to run
 */
void statemachine_set(state_t state){
	next_state = state;
}

/**
 * Gets the current state of the statemachine
 *
 * @return current state
 */
state_t statemachine_get(){
	return next_state;
}

//====================== static functions ==========================//

/**
 * initialize hardware and wait 5 seconds before starting
 */
static void init(){
	init_hardware();

	motor_init();
	motor_set_speed(0);
	motor_drive(FORWARDS);

	servo_init();

	buzzer_init();

	distance_sensor_init();

	char str[] = {"device init, wait 5sec\n"};
	//for (uint8_t i = 0; i < sizeof(str); ++i) {
	//	uart_putc(UART0, (char) str[i]);
	//}
	display_cls();
	display_printf(str);
	serial_printf(str);

	wait_ms(5000);
	display_cls();
	statemachine_set(DRIVE);
}

/**
 * robot drives forward until obstacle detected
 */
static void drive(){
	motor_drive(FORWARDS);
	int16_t distance = distance_sensor_get_distance_cm();

	if (distance < 10){
		motor_set_speed(0);
		//toot toot
		buzzer_on();
		wait_ms(200);
		buzzer_off();
		wait_ms(100);

		buzzer_on();
		wait_ms(200);
		buzzer_off();
		wait_ms(100);

		statemachine_set(LOOK);
	}
	motor_set_speed(255);
	statemachine_set(DRIVE);
}

/**
 * robot stops and looks for the direction with most clearance
 */
static void look(){
	servo_set_angle(-90);
	wait_ms(1000);
	uint16_t distance_left = distance_sensor_get_distance_cm();

	servo_set_angle(0);
	wait_ms(1000);

	servo_set_angle(90);
	wait_ms(1000);
	uint16_t distance_right = distance_sensor_get_distance_cm();

	servo_set_angle(0);

	if (distance_left > 20 || distance_right > 20){
		if (distance_left > distance_right){
			statemachine_set(TLEFT);
		} else {
			statemachine_set(TRIGHT);
		}
	} else {
		statemachine_set(STUCK);
	}
}

/**
 * make the robot turn left about 90 degrees
 */
static void turn_left(){
	motor_set_speed(220);
	motor_drive(ANTICLOCKWISE);
	wait_ms(300);
	motor_set_speed(0);
	statemachine_set(DRIVE);
}

/**
 * make the robot turn right about 90 degrees
 */
static void turn_right(){
	motor_set_speed(220);
	motor_drive(CLOCKWISE);
	wait_ms(300);
	motor_set_speed(0);
	statemachine_set(DRIVE);
}

/**
 * make the robot drive backwards for a moment
 */
static void stuck(){
	motor_drive(BACKWARDS);
	motor_set_speed(255);
	buzzer_on();
	wait_ms(500);
	buzzer_off();
	wait_ms(500);
	buzzer_on();
	wait_ms(500);
	buzzer_off();
	wait_ms(500);
	motor_set_speed(0);
	statemachine_set(LOOK);
}

