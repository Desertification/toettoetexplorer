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

void motor_init(){
	gpio_init(DIR_L);
	gpio_init(DIR_R);

	pwm_set_duty_cycle(PWM_L, 0);

	pwm_set_duty_cycle(PWM_R, 0);

	pwm_set_frequency(20000);
	pwm_init(PWM_R);
	pwm_init(PWM_L);
}

void motor_set_speed(uint8_t value){
	speed = value;
	pwm_set_duty_cycle(PWM_L, value);
	pwm_set_duty_cycle(PWM_R, value);
}

uint8_t motor_get_speed(uint8_t value){
	return speed;
}

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
