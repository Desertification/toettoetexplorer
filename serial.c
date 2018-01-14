/*
 * serial.c
 *
 *  Created on: Jan 2, 2018
 *      Author: thoma
 */

#include "serial.h"
#include "uart.h"
#include <stdarg.h>
#include <stdio.h>

/**
 * send a formatted message through the usb uart connection of the mbed.
 * ex: serial_printf("got %i lines with %s\n",12, "cheeze");
 *
 * @param format format string or message to be sent
 * @param ... arguments to be included in the format string. can be integer, string, double
 */
void serial_printf(char* format,...){
	char buffer[256]; // 256 chars should be enough for most messages
	va_list vlist;
	va_start(vlist, format);
	int length = vsprintf(buffer, format, vlist);
	for (int i = 0; i < length; ++i) {
		uart_putc(UART0, buffer[i]);
	}
	va_end (vlist);
}
