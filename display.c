/*
 * display.c
 *
 *  Created on: Jan 4, 2018
 *      Author: thoma
 */

#include "display.h"
#include "lcd.h"
#include <stdarg.h>
#include <stdio.h>

/**
 * Writes formatted text to the display
 *
 * @param format format string or message to be sent
 * @param ... arguments to be included in the format string. can be integer, string, double
 */
void display_printf(char * format, ...){
	lcd_cmd(0x80);
	char buffer[64];
	va_list vlist;
	va_start(vlist, format);
	int length = vsprintf(buffer, format, vlist);
	for (int i = 0; i < length; ++i) {
		lcd_data(buffer[i]);
	}
	va_end (vlist);
}

/**
 * Clears the display
 */
void display_cls(){
	lcd_cls();
}

