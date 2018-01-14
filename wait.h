/*
 * wait.h
 *
 *  Created on: 15-nov.-2017
 *      Author: robin
 */

#ifndef WAIT_H_
#define WAIT_H_

#include "LPC17xx.h"

/**
 * wait 10 us * a given amount
 *
 * @param amount: number of microseconds to wait / 10
 */
void wait_10us(uint16_t amount);

/**
 * wait a give amount of milliseconds
 *
 * @param amount: number of milliseconds to wait
 */
void wait_ms(uint16_t amount);

#endif /* WAIT_H_ */
