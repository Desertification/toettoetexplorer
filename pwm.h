/*
 * pwm.h
 *
 *  Created on: Oct 11, 2017
 *      Author: thoma
 */

#ifndef PWM_H_
#define PWM_H_

#include "LPC17xx.h"

/**
 * enables the pwm on that pin
 *
 * @param pin pin of the mbed (21-26)
 */
void pwm_init(uint8_t pin);

/*
 * sets the frequency of the pwm
 *
 * @param frequency Hz
 */
void pwm_set_frequency(uint32_t period);

/**
 * returns the configured frequency
 *
 * @return Hz
 */
uint32_t pwm_get_frequency();

/**
 * set the duty cycle of the pwm for that pin
 *
 * @param pin pin of the mbed (21-26)
 * @param value percentage (0-255)
 */
void pwm_set_duty_cycle(uint8_t pin, uint8_t value);

/**
 * get the duty cycle of the pwm for that pin
 *
 * @param pin of the mbed (21-26)
 * @return percentage (0-255)
 */
uint8_t pwm_get_duty_cycle(uint8_t pin);

#endif /* PWM_H_ */
