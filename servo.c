/*
 * servo.c
 *
 *  Created on: Dec 13, 2017
 *      Author: thoma
 */

#include "servo.h"
#include "pwm.h"
#include "wait.h"

static const uint8_t SERVO_PIN = 22;
static uint8_t last_angle = 0;

//====================== forward declarations ======================//

static uint8_t angle_to_duty_cycle(int8_t angle);
static int8_t duty_cycle_to_angle(uint8_t duty_cycle);

//====================== global functions ==========================//

/**
 * Inits the servo
 */
void servo_init(){
	pwm_init(SERVO_PIN);
	servo_set_angle(0);
}

/**
 * Set the angle of the servo
 *
 * @param angle range -90 to 90 degrees
 */
void servo_set_angle(int8_t angle){
	uint8_t dutycycle = angle_to_duty_cycle(angle);
	pwm_set_duty_cycle(SERVO_PIN, dutycycle);
	last_angle = angle;
}

/**
 * Gets the last set angle of the servo
 *
 * @return the last set angle of the servo (-90 to 90)
 */
int8_t servo_get_angle(){
	uint8_t duty_cycle = pwm_get_duty_cycle(SERVO_PIN);
	return duty_cycle_to_angle(duty_cycle);
}

//====================== static functions ==========================//

/**
 * converts the angle to a duty cycle with the appropriate pulse length
 */
uint8_t angle_to_duty_cycle(int8_t angle){
//	-90 = 1ms (left)
//	0 = 1.5ms (central)
//	+90 = 2ms (right)
	double pulse_length = ((angle + 90.0) / 180.0) + 1.0; //in ms
	double period = 1000.0/pwm_get_frequency(); //in ms

	return 255 * (pulse_length/period);
}

/**
 * converts the duty cycle to an angle that corresponds with the pulse length
 */
int8_t duty_cycle_to_angle(uint8_t duty_cycle){
	double period = 1000.0/pwm_get_frequency(); // in ms
	double pulse_length = period*(duty_cycle/255); // in ms

	return (int8_t) ((pulse_length - 1.0) * 180.0) - 90.0;
}
