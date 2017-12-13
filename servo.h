/*
 * servo.h
 *
 *  Created on: Dec 13, 2017
 *      Author: thoma
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "LPC17xx.h"

void servo_init();
void servo_set_angle(int8_t angle);
int8_t servo_get_angle();

#endif /* SERVO_H_ */
