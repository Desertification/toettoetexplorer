/*
 * lcd.h
 *
 *  Created on: Nov 15, 2017
 *      Author: thoma
 */

#ifndef LCD_H_
#define LCD_H_

#include "LPC17xx.h"

void lcd_cmd(uint8_t value);
void lcd_data(uint8_t value);
void lcd_cls(void);
void lcd_reset(void);
void lcd_init(void);

#endif /* LCD_H_ */
