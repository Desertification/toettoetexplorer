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

/**
 * Initialize the motor, set direction forward, set speed 0
 */
void motor_init();

/**
 * Sets the speed of the motor
 *
 * @param value (0 to 255)
 */
void motor_set_speed(uint8_t value);

/**
 * Gets the speed of the motor
 *
 * @return speed of the motor (0 to 255)
 */
uint8_t motor_get_speed();

/**
 * Sets in what direction the motors should drive
 *
 * @param direction (FORWARDS, BACKWARDS, CLOCKWISE, ANTICLOCKWISE)
 */
void motor_drive(motor_dir_t direction);

#endif /* MOTOR_H_ */
