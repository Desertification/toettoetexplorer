/*
 * pwm.c
 *
 *  Created on: Oct 11, 2017
 *      Author: thoma
 *
 *      p26		pwm1.1
 *      p25		pwm1.2
 *      p24		pwm1.3
 *      p23		pwm1.4
 *      p22		pwm1.5
 *      p21		pwm1.6
 */

#include "pwm.h"

static uint32_t pclk_freq = 96000000/4; // default for pwm
static volatile uint32_t *match_lut[] = { // match register lookup table
	&LPC_PWM1->MR1, &LPC_PWM1->MR2, &LPC_PWM1->MR3,
	&LPC_PWM1->MR4, &LPC_PWM1->MR5, &LPC_PWM1->MR6
};

//====================== forward declarations ======================//

static uint8_t is_pin_valid(uint8_t pin);
static uint8_t pin_to_index(uint8_t pin);

static void config_pinsel_pwm(uint8_t pin);
static void config_power_ctrl_pwm();
static void config_pinmode_none(uint8_t pin);

static void config_enable_pwm_output(uint8_t pin);
static void config_single_edge(uint8_t pin);
static void config_reset_on_PWMMR0();
static void config_timer_pwm_mode();
static void config_enable_counter();

static void load_new_duty_cycle(uint8_t pin);
static void load_new_period();

//====================== global functions ======================//

/**
 * enables the pwm on that pin
 *
 * @param pin pin of the mbed (21-26)
 */
void pwm_init(uint8_t pin){
	if (is_pin_valid(pin)){
		config_power_ctrl_pwm();
		config_pinmode_none(pin);
		config_pinsel_pwm(pin);

		config_enable_pwm_output(pin);
		config_single_edge(pin);
		config_reset_on_PWMMR0();
		config_timer_pwm_mode();
		config_enable_counter();
	}
}

/*
 * sets the frequency of the pwm
 *
 * @param frequency Hz
 */
void pwm_set_frequency(uint32_t frequency){
	// todo more DRY solution
	uint8_t d21 = pwm_get_duty_cycle(21);
	uint8_t d22 = pwm_get_duty_cycle(22);
	uint8_t d23 = pwm_get_duty_cycle(23);
	uint8_t d24 = pwm_get_duty_cycle(24);
	uint8_t d25 = pwm_get_duty_cycle(25);
	uint8_t d26 = pwm_get_duty_cycle(26);

	LPC_PWM1->MR0 = pclk_freq / frequency;
	load_new_period();

	// todo more DRY solution
	pwm_set_duty_cycle(21, d21);
	pwm_set_duty_cycle(22, d22);
	pwm_set_duty_cycle(23, d23);
	pwm_set_duty_cycle(24, d24);
	pwm_set_duty_cycle(25, d25);
	pwm_set_duty_cycle(26, d26);
}

/**
 * returns the configured frequency
 *
 * @return Hz
 */
uint32_t pwm_get_frequency(){
	return pclk_freq / LPC_PWM1->MR0;
}

/**
 * set the duty cycle of the pwm for that pin
 *
 * @param pin pin of the mbed (21-26)
 * @param value percentage (0-255)
 */
void pwm_set_duty_cycle(uint8_t pin, uint8_t value){
	if (is_pin_valid(pin)){
		uint8_t index = pin_to_index(pin);
		double percentage = value/255.0;
		uint32_t match_value = ((pclk_freq * percentage) / pwm_get_frequency());
		*match_lut[index] = match_value;
		load_new_duty_cycle(pin);
	}
}

/**
 * get the duty cycle of the pwm for that pin
 *
 * @param pin of the mbed (21-26)
 * @return percentage (0-255)
 */
uint8_t pwm_get_duty_cycle(uint8_t pin){
	if (is_pin_valid(pin)){
		uint8_t index = pin_to_index(pin);
		uint32_t match_value = *match_lut[index];
		return (uint32_t) ((match_value * pwm_get_frequency() * 255.0) / pclk_freq);
	} else {
		return 0;
	}
}

//====================== static functions ======================//

/**
 * checks if the pin can function as pwm
 */
uint8_t is_pin_valid(uint8_t pin){
	return pin >= 21 && pin <= 26;
}

/**
 * convert pin number to index (for lut)
 */
static uint8_t pin_to_index(uint8_t pin){
	switch (pin) {
		case 26:
			return 0;
		case 25:
			return 1;
		case 24:
			return 2;
		case 23:
			return 3;
		case 22:
			return 4;
		case 21:
			return 5;
		default:
			return 0; // error
	}
}

/**
 * set the pin to pwm function (function 2)
 */
void config_pinsel_pwm(uint8_t pin){
	uint8_t index = pin_to_index(pin) << 1; // 2 bits are reserved in the register per pin

	LPC_PINCON->PINSEL4 &= ~(3 << index); // clear to 00
	LPC_PINCON->PINSEL4 |=  (1 << index); // set to 01
}

/**
 * enable power and clock for pwm module
 */
void config_power_ctrl_pwm(){
	LPC_SC->PCONP |= (1 << 6); // pwm1 power/clock control bit
}

/**
 * disable pull-up mode on the pin
 */
static void config_pinmode_none(uint8_t pin){
	uint8_t index = pin_to_index(pin) << 1; // 2 bits are reserved in the register per pin

	LPC_PINCON->PINMODE4 &= ~(3 << index); // clear to 00
	LPC_PINCON->PINMODE4 |=  (2 << index); // set to 10
}

/**
 * enable the pwm output in the control register
 */
void config_enable_pwm_output(uint8_t pin){
	uint8_t index = pin_to_index(pin) + 9; //PWMENA starts at bit 9

	LPC_PWM1->PCR |= (1 << index);
}

/**
 * set the pwm to single edge controlled mode
 */
void config_single_edge(uint8_t pin){
	uint8_t index = pin_to_index(pin);

	if(index != 0) // unused for pwm1
		LPC_PWM1->PCR &= ~(1 << index);
}

/**
 * set pwm to reset the timer when PWMMR0 matches
 */
static void config_reset_on_PWMMR0(){
	LPC_PWM1->MCR |= 2;
}

/**
 * sets timer to pwm mode instead of timer mode
 */
static void config_timer_pwm_mode(){
	LPC_PWM1->TCR |= (1 << 3);
}

/**
 * enable the pwm counter and prescaler for counting
 */
static void config_enable_counter(){
	LPC_PWM1->TCR |= 1;
}

/**
 * let the written value to the match register be effective at the next cycle
 */
static void load_new_duty_cycle(uint8_t pin){
	int index = pin_to_index(pin) + 1;
	LPC_PWM1->LER |= (1 << index);
}

/**
 * let the written value to the match register be effective at the next cycle
 */
static void load_new_period(){
	LPC_PWM1->LER |= 1;
}

