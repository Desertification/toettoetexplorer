/*
 * GPIO_Driver.c
 *
 *  Created on: 11-okt.-2017
 *      Author: robin
 */
#include "GPIO_Driver.h"

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

void GPIO_init(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		set_mode(pin, 0);
		set_direction(pin, 1);
		clear(pin);
	}
}

/* pinmodes
 * 0 = pull-up resistor
 * 1 = repeater mode
 * 2 = neither pull-up or pull-down
 * 3 = pull-down
 */
void set_mode(uint8_t pin, uint8_t mode) {
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

uint8_t get_mode(uint8_t pin) {
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
}

//0 is input
//1 is output
void set_direction(uint8_t pin, uint8_t direction) {
	if ( is_pin_valid(pin) ) {
		if ( direction ) {
			select_LPC_GPIO(pin)->FIODIR |= 1 << port_index(pin);
		}
		else {
			select_LPC_GPIO(pin)->FIODIR &= ~(1 << port_index(pin));
		}
	}
}

uint8_t get_direction(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		return (select_LPC_GPIO(pin)->FIODIR >> port_index(pin)) & 1;
	}
}

void set(uint8_t pin) {
	if ( is_pin_valid(pin) && is_pin_output(pin) ) {
		select_LPC_GPIO(pin)->FIOSET |= 1 << port_index(pin);
	}
}

void clear(uint8_t pin) {
	if ( is_pin_valid(pin) && is_pin_output(pin) ) {
		select_LPC_GPIO(pin)->FIOCLR |= 1 << port_index(pin);
	}
}

void toggle(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		uint8_t current = read(pin);
		if (current) { clear(pin); }

		else { set(pin); }
	}
}

uint8_t read(uint8_t pin) {
	if ( is_pin_valid(pin) ) {
		return select_LPC_GPIO(pin)->FIOPIN >>  port_index(pin) & 1;
	}
}

LPC_GPIO_TypeDef* select_LPC_GPIO(uint8_t pin) {
	uint8_t port = mbed_pin_to_port_lookup[pin];
	return LPC_GPIO_lookup[ port / 100 ];
}

uint8_t port_index(uint8_t pin) {
	return (mbed_pin_to_port_lookup[pin] % 100);
}

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

uint8_t is_pin_valid(uint8_t pin) {
	return pin >= 5 && pin <= 30;
}

uint8_t is_pin_output(uint8_t pin) {
	return get_direction(pin) == 1;
}
