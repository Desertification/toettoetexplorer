/*
 * gpio.c
 *
 *  Created on: 11-okt.-2017
 *      Author: robin
 */
#include "gpio.h"


//Different pin modes
const uint8_t GPIO_PINMODE_PULLUP = 0;
const uint8_t GPIO_PINMODE_REPEATER = 1;
const uint8_t GPIO_PINMODE_NO_PULL = 2;
const uint8_t GPIO_PINMODE_PULLDOWN = 3;

//Different directions for pins
const uint8_t GPIO_PINDIRECTION_INPUT = 0;
const uint8_t GPIO_PINDIRECTION_OUTPUT = 1;


//forward declarations
static LPC_GPIO_TypeDef* select_LPC_GPIO(uint8_t pin);
static uint8_t port_index(uint8_t pin);
static volatile uint32_t* select_PINMODE(uint8_t pin);
static uint8_t is_pin_valid(uint8_t pin);
static uint8_t is_pin_output(uint8_t pin);

static const LPC_GPIO_TypeDef* LPC_GPIO_lookup[5] =
{
	LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4,
};

//value 255: pin does not exist (pin 0) or should not be accessed.
//translation goes as example: pin 24 -> P2.2 -> 202
// 9 = P0.09
// 23 = P0.23
static const uint8_t mbed_pin_to_port_lookup[31] = {
	255, 255, 255, 255, 255, 9, 8, 7, 6, 0, 1, 18, 17, 15, 16, 23, 24, 25, 26, //pin (0) + 1-18
	130, 131, 205, 204, 203, 202, 201, 200, 11, 10, 5, 4 //pin 19-30
};

/**
 * initializes mbed pin as GPIO
 *
 * @param pin: mbed pin to initialize
 */
void gpio_init(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		gpio_set_mode(pin, GPIO_PINMODE_PULLUP);
		gpio_set_direction(pin, GPIO_PINDIRECTION_OUTPUT);
		gpio_clear(pin);
	}
}

/**
 * set GPIO pinmode for a mbed pin
 *
 * @param pin: mbed pin
 * @param mode: choose pin mode
 */
void gpio_set_mode(uint8_t pin, uint8_t mode) {
	if ( is_pin_valid(pin) ) {
		uint8_t shift;
		if ( port_index(pin) >= 0 && port_index(pin) < 16 ) {
			shift = 2 * port_index(pin);
		}
		if ( port_index(pin) >= 16 && port_index(pin) < 32 ) {
			shift = 2 * (port_index(pin) - 16);
		}
		*select_PINMODE(pin) &= ~(3 << shift );
		*select_PINMODE(pin) |= mode << shift;
	}
}

/**
 * get GPIO pinmode from a mbed pin
 *
 * @param pin: mbed pin
 * @return pinmode value or value 255 when invalid pin was entered
 */
uint8_t gpio_get_mode(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		uint8_t shift;
		if ( port_index(pin) >= 0 && port_index(pin) < 16 ) {
			shift = 2 * port_index(pin);
		}
		if ( port_index(pin) >= 16 && port_index(pin) < 32 ) {
			shift = 2 * (port_index(pin) - 16);
		}
		return *select_PINMODE(pin) >> shift & 1;
	}
	return 255;
}

/**
 * initializes GPIO direction for mbed pin
 *
 * @param pin: mbed pin to initialize GPIO direction
 */
void gpio_set_direction(uint8_t pin, uint8_t direction) {
	if ( is_pin_valid(pin) ) {
		if ( direction ) {
			select_LPC_GPIO(pin)->FIODIR |= 1 << port_index(pin);
		}
		else {
			select_LPC_GPIO(pin)->FIODIR &= ~(1 << port_index(pin));
		}
	}
}

/**
 * get GPIO direction from mbed pin
 *
 * @param pin: mbed pin to get GPIO direction from
 * @return pin direction value or 255 when invalid pin was entered
 */
uint8_t gpio_get_direction(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		return (select_LPC_GPIO(pin)->FIODIR >> port_index(pin)) & 1;
	}
	return 255;
}

/**
 * set mbed pin high
 *
 * @param pin: mbed pin to set
 */
void gpio_set(uint8_t pin) {
	if ( is_pin_valid(pin) && is_pin_output(pin) ) {
		select_LPC_GPIO(pin)->FIOSET |= 1 << port_index(pin);
	}
}

/**
 * set mbed pin low
 *
 * @param pin: pin to clear
 */
void gpio_clear(uint8_t pin) {
	if ( is_pin_valid(pin) && is_pin_output(pin) ) {
		select_LPC_GPIO(pin)->FIOCLR |= 1 << port_index(pin);
	}
}

/**
 * toggle mbed pin
 *
 * @param pin: mbed pin to toggle
 */
void gpio_toggle(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		uint8_t current = gpio_read(pin);
		if (current) {
			gpio_clear(pin);
		}
		else {
			gpio_set(pin);
		}
	}
}

/**
 * writes a value to mbed pin
 *
 * @param pin: mbed pin to write value to
 * @param value: value to write to pin (0 = low, anything else is high)
 */
void gpio_write(uint8_t pin, uint8_t value){
	switch (value) {
		case 0:
			gpio_clear(pin);
			break;
		default:
			gpio_set(pin);
			break;
	}
}

/**
 * read GPIO value from mbed pin
 *
 * @param pin: mbed pin to read from
 */
uint8_t gpio_read(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		return select_LPC_GPIO(pin)->FIOPIN >>  port_index(pin) & 1;
	}
	return 255;
}

/**
 * enables rising edge interrupt on mbed pin
 *
 * @param pin: mbed pin
 */
void gpio_enable_interrupt_rising_edge(uint8_t pin) {
	uint8_t port = mbed_pin_to_port_lookup[pin];
	if ( port >= 0 && port < 100) {
		LPC_GPIOINT->IO0IntEnR |= 1 << port_index(pin);
	}
}

/**
 * enables falling edge interrupt on mbed pin
 *
 * @param pin: mbed pin
 */
void gpio_enable_interrupt_falling_edge(uint8_t pin) {
	uint8_t port = mbed_pin_to_port_lookup[pin];
	if ( port >= 0 && port < 100) {
		LPC_GPIOINT->IO0IntEnF |= 1 << port_index(pin);
	}
}

/**
 * uses mbed pin to select the right GPIO register to use in functions
 *
 * @param pin: mbed pin
 * @return the correct GPIO register
 */
LPC_GPIO_TypeDef* select_LPC_GPIO(uint8_t pin) {
	uint8_t port = mbed_pin_to_port_lookup[pin];
	return (LPC_GPIO_TypeDef*) LPC_GPIO_lookup[ port / 100 ];
}

/**
 * links the given mbed pin to his internal port number
 *
 * @param pin: mbed pin
 * @return the portnumber for the given pin
 */
uint8_t port_index(uint8_t pin) {
	return (mbed_pin_to_port_lookup[pin] % 100);
}

/**
 * Select right PINMODE register for a given pin.
 *
 * @param pin: mbed pin
 * @return the right PINMODE register
 */
volatile uint32_t* select_PINMODE(uint8_t pin) {
	uint8_t port = mbed_pin_to_port_lookup[pin];
	if ( port >= 0 && port < 16 ) {
		return &LPC_PINCON->PINMODE0;
	}
	else if( port >= 16 && port < 32 ) {
		return &LPC_PINCON->PINMODE1;
	}
	else if ( port >= 116 && port < 132 ) {
		return &LPC_PINCON->PINMODE3;
	}
	else if ( port >= 200 && port < 216 ) {
		return &LPC_PINCON->PINMODE4;
	}
	else { return 0;}
}

/**
 * validates if the requested pin is allowed to be accessed/changed (see top for valid pins)
 *
 * @param pin: mbed pin
 * @return true = valid pin, false = invalid pin
 */
uint8_t is_pin_valid(uint8_t pin) {
	return pin >= 5 && pin <= 30;
}

/**
 * return is pin is output or not
 *
 * @param pin: mbed pin
 * @return true = output, false = input
 */
uint8_t is_pin_output(uint8_t pin) {
	return gpio_get_direction(pin) == 1;
}
