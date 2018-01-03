/*
 * servo.h
 *
 *  Created on: Dec 13, 2017
 *      Author: thoma
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "LPC17xx.h"

/**
 * Inits the servo
 */
void servo_init();

/**
 * Set the angle of the servo
 *
 * @param angle range -90 to 90 degrees
 */
void servo_set_angle(int8_t angle);

/**
 * Gets the last set angle of the servo
 *
 * @return the last set angle of the servo (-90 to 90)
 */
int8_t servo_get_angle();

#endif /* SERVO_H_ */
