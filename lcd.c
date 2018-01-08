/*
 * lcd.c
 *
 *  Created on: Nov 15, 2017
 *      Author: thoma
 */

#include "lcd.h"
#include "gpio.h"
#include "wait.h"

/**
 * lcd pins with corresponding mbed pin
 */
static enum lcd {
	RS = 11, EN = 12, D0 = 15,D1 = 16,D2 = 17, D3 = 18
};

/**
 * sends a command to the lcd screen to perform an action
 *
 * @param value: command to send (commands consist of a 8-bit unsigned integer)
 */
 void lcd_cmd(uint8_t value) {
	gpio_clear(RS);//RS low = instruction input

	//Using 4-bit mode we need to split the instruction. Start with higher nibble
    uint8_t data = value >> 4;
    gpio_write(D0, data & (1 << 0));
    gpio_write(D1, data & (1 << 1));
	gpio_write(D2, data & (1 << 2));
	gpio_write(D3, data & (1 << 3));

    //enable signal
    wait_10us(4);
    gpio_clear(EN);
    wait_10us(4);
    gpio_set(EN);

    //second nibble
    gpio_write(D0, value & (1 << 0));
    gpio_write(D1, value & (1 << 1));
    gpio_write(D2, value & (1 << 2));
    gpio_write(D3, value & (1 << 3));

    //enable signal
    wait_10us(4);
    gpio_clear(EN);
    wait_10us(4);
    gpio_set(EN);
}

 /**
  * sends data to put on screen
  *
  * @param value: command to send (commands consist of a 8-bit unsigned integer)
  */
void lcd_data(uint8_t value) {
    gpio_set(RS);
    //Using 4-bit mode we need to split the data. Start with higher nibble
    uint8_t data = value >> 4;
    gpio_write(D0, data & (1 << 0));
    gpio_write(D1, data & (1 << 1));
    gpio_write(D2, data & (1 << 2));
    gpio_write(D3, data & (1 << 3));

    //enable signal
    wait_10us(4);
    gpio_clear(EN);
    wait_10us(4);
    gpio_set(EN);

    //second nibble
    gpio_write(D0, value & (1 << 0));
    gpio_write(D1, value & (1 << 1));
    gpio_write(D2, value & (1 << 2));
    gpio_write(D3, value & (1 << 3));

    //enable signal
    wait_10us(4);
    gpio_clear(EN);
    wait_10us(4);
    gpio_set(EN);
}

/**
 * clear display (also DDRAM content)
 */
void lcd_cls(void) {
    lcd_cmd(0x01);
    wait_ms(2);
}

/**
 * reset lcd
 */
void lcd_reset(void){
    gpio_set(EN);
    gpio_clear(RS);
    wait_ms(20);        // **Wait 15ms to ensure powered up
    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)

    for (int i=0; i < 3; i++) {
        lcd_cmd(0x3);
        wait_ms(2);  // this command takes 1.64ms, so wait for it
    }
    lcd_cmd(0x2);     // 4-bit mode
    wait_10us(4);    // most instructions take 40us
    lcd_cmd(0x28); // Function set 001 BW N F - -
    lcd_cmd(0x0c);
    lcd_cmd(0x06);  // Cursor Direction and Display Shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes
    lcd_cls();
}

/**
 * initialises the lcd
 */
void lcd_init(void) {
	//initialize pins
	gpio_init(D0);
	gpio_init(D1);
	gpio_init(D2);
	gpio_init(D3);
	gpio_init(RS);
	gpio_init(EN);

	//set pin modes
	gpio_set_mode(D0, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(D1, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(D2, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(D3, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(RS, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(EN, GPIO_PINMODE_NO_PULL);

	//reset lcd
    lcd_reset();
    // - Function set
    // - DL = 0 -> 4bit mode
    // - N = 1 -> 2 lines
    // - F = 0 -> 5*7 dots GCROM
    // - # = 0 no 24*4 module
    lcd_cmd(0x28);
    // - Display On/Off control
    // - D = 1 -> Display is on
    // - C = 0 -> Cursor off
    // - B = 0 -> Cursor blink off
    lcd_cmd(0x0C);
    // - Entry mode set
    // - I/D = 1 -> increment cursor position
    // - S = 0 -> no display shift
    lcd_cmd(0x06);
    // - GCRAM address
    // - 0x80 + address = 0b1AAA_AAAA
    // - setAddres to 0
    lcd_cmd(0x01);
    wait_ms(2);
}
