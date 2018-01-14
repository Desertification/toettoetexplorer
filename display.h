/*
 * display.h
 *
 *  Created on: Jan 4, 2018
 *      Author: thoma
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/**
 * Writes formatted text to the display
 *
 * @param format format string or message to be sent
 * @param ... arguments to be included in the format string. can be integer, string, double
 */
void display_printf(char * format, ...);

/**
 * Clears the display
 */
void display_cls();

#endif /* DISPLAY_H_ */
