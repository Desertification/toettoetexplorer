/*
 * buzzer.h
 *
 *  Created on: Dec 19, 2017
 *      Author: thoma
 */

#include "LPC17xx.h"

#ifndef BUZZER_H_
#define BUZZER_H_

/**
 * initializes the buzzer
 */
void buzzer_init();

/**
 * turns the buzzer on
 */
void buzzer_on();

/**
 * turns the buzzer off
 */
void buzzer_off();

/**
 * Returns if the buzzer is on.
 *
 * @return if the buzzer is on
 */
uint8_t buzzer_is_on();


#endif /* BUZZER_H_ */
