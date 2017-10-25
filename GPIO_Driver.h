/*
 * GPIO_Driver.h
 *
 *  Created on: 11-okt.-2017
 *      Author: robin
 */

#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#include "LPC17xx.h"

void GPIO_init(uint8_t pin);
void set_mode(uint8_t pin, uint8_t mode);
void set_direction(uint8_t pin, uint8_t direction);

uint8_t get_mode(uint8_t pin);
uint8_t get_direction(uint8_t pin);

void set(uint8_t pin);
void clear(uint8_t pin);
void toggle(uint8_t pin);
uint8_t read(uint8_t pin);

void enable_interrupt(uint8_t pin);

#endif /* GPIO_DRIVER_H_ */
