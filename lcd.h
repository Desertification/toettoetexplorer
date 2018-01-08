/*
 * lcd.h
 *
 *  Created on: Nov 15, 2017
 *      Author: thoma
 */

#ifndef LCD_H_
#define LCD_H_

#include "LPC17xx.h"

/**
 * sends a command to the lcd screen to perform an action
 *
 * @param value: command to send (commands consist of a 8-bit unsigned integer)
 */
void lcd_cmd(uint8_t value);

/**
 * sends data to put on screen
 *
 * @param value: command to send (commands consist of a 8-bit unsigned integer)
 */
void lcd_data(uint8_t value);

/**
 * clear display (also DDRAM content)
 */
void lcd_cls(void);

/**
 * reset lcd
 */
void lcd_reset(void);

/**
 * initialises the lcd
 */
void lcd_init(void);

#endif /* LCD_H_ */
