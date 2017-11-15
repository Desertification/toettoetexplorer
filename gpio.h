/*
 * gpio.h
 *
 *  Created on: 11-okt.-2017
 *      Author: robin
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "LPC17xx.h"

//Different modes for pins
extern const uint8_t GPIO_PINMODE_PULLUP;
extern const uint8_t GPIO_PINMODE_REPEATER;
extern const uint8_t GPIO_PINMODE_NO_PULL;
extern const uint8_t GPIO_PINMODE_PULLDOWN;

//Different directions for pins
extern const uint8_t GPIO_PINDIRECTION_INPUT;
extern const uint8_t GPIO_PINDIRECTION_OUTPUT;

void gpio_init(uint8_t pin);
void gpio_set_mode(uint8_t pin, uint8_t mode);
void gpio_set_direction(uint8_t pin, uint8_t direction);

uint8_t gpio_get_mode(uint8_t pin);
uint8_t gpio_get_direction(uint8_t pin);

void gpio_set(uint8_t pin);
void gpio_clear(uint8_t pin);
void gpio_toggle(uint8_t pin);
void gpio_write(uint8_t pin, uint8_t b);
uint8_t gpio_read(uint8_t pin);

void gpio_enable_interrupt_rising_edge(uint8_t pin);

#endif /* GPIO_H_ */
