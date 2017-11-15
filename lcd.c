/*
 * lcd.c
 *
 *  Created on: Nov 15, 2017
 *      Author: thoma
 */

#include "lcd.h"
#include "gpio.h"

static enum lcd {
	RS = 11, EN = 12, D0 = 15,D1 = 16,D2 = 17, D3 = 18
};

 void lcd_cmd(uint8_t value) {
	gpio_clear(RS);

    uint8_t data = value >> 4;
    gpio_write(D0, data & (1 << 0));
    gpio_write(D1, data & (1 << 1));
	gpio_write(D2, data & (1 << 2));
	gpio_write(D3, data & (1 << 3));

    //instructie neemt 40µs tijd
    wait_us(40);
    gpio_clear(EN);
    wait_us(40);
    gpio_set(EN);

    gpio_write(D0, value & (1 << 0));
    gpio_write(D1, value & (1 << 1));
    gpio_write(D2, value & (1 << 2));
    gpio_write(D3, value & (1 << 3));

    wait_us(40);
    gpio_clear(EN);
    wait_us(40);
    gpio_set(EN);
}

void lcd_data(uint8_t value) {
    gpio_set(RS);
    //zend eerste 4bit
    uint8_t data = value >> 4;
    gpio_write(D0, data & (1 << 0));
    gpio_write(D1, data & (1 << 1));
    gpio_write(D2, data & (1 << 2));
    gpio_write(D3, data & (1 << 3));

    //instructie neemt 40µs tijd
    wait_us(40);
    //laad de data in door H -> L
    gpio_clear(EN);
    wait_us(40);
    gpio_set(EN);
    //zend tweede 4bit
    gpio_write(D0, value & (1 << 0));
    gpio_write(D1, value & (1 << 1));
    gpio_write(D2, value & (1 << 2));
    gpio_write(D3, value & (1 << 3));

    wait_us(40);
    gpio_clear(EN);
    wait_us(40);
    gpio_set(EN);
}

void lcd_cls(void) {
    lcd_cmd(0x01);
    wait_ms(2);
}

void lcd_reset(void){
    gpio_set(EN);
    gpio_clear(RS);
    wait(0.015);        // Wait 15ms to ensure powered up
    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    for (int i=0; i < 3; i++) {
        lcd_cmd(0x3);
        wait(0.00164);  // this command takes 1.64ms, so wait for it
    }
    lcd_cmd(0x2);     // 4-bit mode
    wait(0.000040f);    // most instructions take 40us
    lcd_cmd(0x28); // Function set 001 BW N F - -
    lcd_cmd(0x0C);
    lcd_cmd(0x6);  // Cursor Direction and Display Shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes
    lcd_cls();
}

void lcd_init(void) {
	// DATA
	gpio_init(D0);
	gpio_init(D1);
	gpio_init(D2);
	gpio_init(D3);
	gpio_init(RS);
	gpio_init(EN);

	gpio_set_mode(D0, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(D1, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(D2, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(D3, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(RS, GPIO_PINMODE_NO_PULL);
	gpio_set_mode(EN, GPIO_PINMODE_NO_PULL);


    lcd_reset();
    // - Function set
    // - DL = 0 -> 4bit mode
    // - N = 1 -> 2 lines
    // - F = 0 -> 5*7 dots GCROM
    // - # = 0 geen 24*4 module
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
    // - GCRAM adress
    // - 0x80 + adres = 0b1AAA_AAAA
    // - setAddres to 0
    lcd_cmd(0x01);
    wait(0.00164);
}
/*
int main() {
    lcd_init();
    lcd_cmd(0x80); // set pos 0
    lcd_data('A');
    lcd_data('l');
    lcd_data('e');
    lcd_data('x');
    lcd_data('a');
    lcd_data('n');
    lcd_data('d');
    lcd_data('e');
    lcd_data('r');
    for(int i =0; i < 10; i++)
    {
        wait(1);
        lcd_cmd(0x1C);
    }
    //lcd.printf("Welcome !!!");
}
*/
