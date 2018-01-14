/*
 * serial.h
 *
 *  Created on: Jan 2, 2018
 *      Author: thoma
 */

#ifndef SERIAL_H_
#define SERIAL_H_

/**
 * send a formatted message through the usb uart connection of the mbed.
 * ex: serial_printf("got %i lines with %s\n",12, "cheeze");
 *
 * @param format format string or message to be sent
 * @param ... arguments to be included in the format string. can be integer, string, double
 */
void serial_printf(char* format,...);

#endif /* SERIAL_H_ */
