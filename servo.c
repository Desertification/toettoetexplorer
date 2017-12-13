/*
 * servo.c
 *
 *  Created on: Dec 13, 2017
 *      Author: thoma
 */

#include "servo.h"
#include "pwm.h"

static const uint8_t SERVO_PIN = 22;

//====================== forward declarations ======================//

uint8_t angle_to_dutycycle(int8_t angle);
int8_t dutycycle_to_angle(uint8_t dutycycle);

//====================== global functions ==========================//
void servo_init(){
	pwm_init(SERVO_PIN);
	servo_set_angle(0);
}

void servo_set_angle(int8_t angle){
	uint8_t dutycycle = angle_to_dutycycle(angle);
	pwm_set_duty_cycle(SERVO_PIN, dutycycle);
}

int8_t servo_get_angle(){
	uint8_t dutycycle = pwm_get_duty_cycle(SERVO_PIN);
}

//====================== static functions ==========================//

uint8_t angle_to_dutycycle(int8_t angle){
//	-90 = 1ms (left)
//	0 = 1.5ms (central)
//	+90 = 2ms (right)
	double pulse_length = ((angle + 90.0) / 180.0) + 1.0; //in ms
	double period = 1000.0/pwm_get_frequency(); //in ms

	return 255 * (pulse_length/period);
}

int8_t dutycycle_to_angle(uint8_t dutycycle){
	//int8_t angle = (int8_t) ((period - 1) * 90) - 90;
}
