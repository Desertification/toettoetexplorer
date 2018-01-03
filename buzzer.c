/*
 * buzzer.c
 *
 *  Created on: Dec 19, 2017
 *      Author: thoma
 */

#include "buzzer.h"
#include "gpio.h"

static const uint8_t BUZZER_PIN = 25;

//====================== forward declarations ======================//

//====================== global functions ==========================//

/**
 * initializes the buzzer
 */
void buzzer_init(){
	gpio_init(BUZZER_PIN);
	gpio_set_mode(BUZZER_PIN, GPIO_PINMODE_NO_PULL);
}

/**
 * turns the buzzer on
 */
void buzzer_on(){
	gpio_set(BUZZER_PIN);
}

/**
 * turns the buzzer off
 */
void buzzer_off(){
	gpio_clear(BUZZER_PIN);
}

/**
 * Returns if the buzzer is on.
 *
 * @return if the buzzer is on
 */
uint8_t buzzer_is_on(){
	return gpio_read(BUZZER_PIN);
}

//====================== static functions ==========================//
