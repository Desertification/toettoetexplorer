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

/**
 * initializes mbed pin as GPIO
 *
 * @param pin: mbed pin to initialize
 */
void gpio_init(uint8_t pin);

/**
 * set GPIO pinmode for a mbed pin
 *
 * @param pin: mbed pin
 * @param mode: choose pin mode
 */
void gpio_set_mode(uint8_t pin, uint8_t mode);

/**
 * initializes GPIO direction for mbed pin
 *
 * @param pin: mbed pin to initialize GPIO direction
 */
void gpio_set_direction(uint8_t pin, uint8_t direction);

/**
 * get GPIO pinmode from a mbed pin
 *
 * @param pin: mbed pin
 * @return pinmode value or value 255 when invalid pin was entered
 */
uint8_t gpio_get_mode(uint8_t pin);

/**
 * get GPIO direction from mbed pin
 *
 * @param pin: mbed pin to get GPIO direction from
 * @return pin direction value or 255 when invalid pin was entered
 */
uint8_t gpio_get_direction(uint8_t pin);

/**
 * set mbed pin high
 *
 * @param pin: mbed pin to set
 */
void gpio_set(uint8_t pin);

/**
 * set mbed pin low
 *
 * @param pin: pin to clear
 */
void gpio_clear(uint8_t pin);

/**
 * toggle mbed pin
 *
 * @param pin: mbed pin to toggle
 */
void gpio_toggle(uint8_t pin);

/**
 * writes a value to mbed pin
 *
 * @param pin: mbed pin to write value to
 * @param value: value to write to pin (0 = low, anything else is high)
 */
void gpio_write(uint8_t pin, uint8_t b);

/**
 * read GPIO value from mbed pin
 *
 * @param pin: mbed pin to read from
 */
uint8_t gpio_read(uint8_t pin);

/**
 * enables rising edge interrupt on mbed pin
 *
 * @param pin: mbed pin
 */
void gpio_enable_interrupt_rising_edge(uint8_t pin);

/**
 * enables falling edge interrupt on mbed pin
 *
 * @param pin: mbed pin
 */
void gpio_enable_interrupt_falling_edge(uint8_t pin);

#endif /* GPIO_H_ */
