/*
 * init.c
 *
 *  Created on: Dec 13, 2017
 *      Author: thoma
 */

#include "init.h"
#include "pwm.h"
#include "uart.h"
#include "lcd.h"

/**
 * Initializes hardware with shared resources or properties
 */
void init_hardware(){
	pwm_set_frequency(200); //frequency is shared by all the pwm devices

	// init uart log device so this does not need to be repeated in the serial mid layer
	uart_init(UART0);
	uart_set_baud(UART0, 115200);
	uart_set_parity(UART0, NONE);
	uart_set_data_bits(UART0, EIGHT);

	// init lcd so this does not need to be repeated in the display mid layer
	lcd_init();
}
