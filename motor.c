/*
 * motor.c
 *
 *  Created on: Oct 25, 2017
 *      Author: thoma
 */
#include "motor.h"
#include "pwm.h"
#include "gpio.h"

static const uint8_t PWM_L = 23;
static const uint8_t PWM_R = 24;

static const uint8_t DIR_L = 20;
static const uint8_t DIR_R = 19;

static uint8_t speed = 0; // track requested for when there is correction in one of the wheels so the pwm value is not the same as this

//====================== global functions ======================//

/**
 * Initialize the motor, set direction forward, set speed 0
 */
void motor_init(){
	gpio_init(DIR_L);
	gpio_init(DIR_R);

	pwm_set_duty_cycle(PWM_L, 0);

	pwm_set_duty_cycle(PWM_R, 0);

	pwm_init(PWM_R);
	pwm_init(PWM_L);
}

/**
 * Sets the speed of the motor
 *
 * @param value (0 to 255)
 */
void motor_set_speed(uint8_t value){
	speed = value;
	pwm_set_duty_cycle(PWM_L, value);
	pwm_set_duty_cycle(PWM_R, value);
}

/**
 * Gets the speed of the motor
 *
 * @return speed of the motor (0 to 255)
 */
uint8_t motor_get_speed(){
	return speed;
}

/**
 * Sets in what direction the motors should drive
 *
 * @param direction (FORWARDS, BACKWARDS, CLOCKWISE, ANTICLOCKWISE)
 */
void motor_drive(motor_dir_t direction){
	switch (direction) {
		case CLOCKWISE:
			gpio_clear(DIR_L);
			gpio_set(DIR_R);
			break;
		case ANTICLOCKWISE:
			gpio_set(DIR_L);
			gpio_clear(DIR_R);
			break;
		case BACKWARDS:
			gpio_set(DIR_L);
			gpio_set(DIR_R);
			break;
		case FORWARDS:
		default: // forwards
			gpio_clear(DIR_L);
			gpio_clear(DIR_R);
			break;
	}
}
