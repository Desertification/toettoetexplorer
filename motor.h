/*
 * motor.h
 *
 *  Created on: Oct 25, 2017
 *      Author: thoma
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "LPC17xx.h"

typedef enum motor_dir{
	FORWARDS,
	BACKWARDS,
	CLOCKWISE,
	ANTICLOCKWISE
} motor_dir_t;

void motor_init();

void motor_set_speed(uint8_t value);

uint8_t motor_get_speed(uint8_t value);

void motor_drive(motor_dir_t direction);

#endif /* MOTOR_H_ */
