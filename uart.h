/*
 * uart.h
 *
 *  Created on: Nov 8, 2017
 *      Author: thoma
 */

#include "LPC17xx.h"

#ifndef UART_H_
#define UART_H_

// UART0	tx	p0.2		rx	p0.3		pin	USBTX, USBRX
// UART1	tx	p0.15		rx	p0.16		pin	p13, p14
// UART2	tx	p0.10		rx	p0.11		pin	p28, p27
// UART3	tx	p0.0		rx	p0.1		pin	p9, p10
typedef enum uart_device {
	UART0, UART1, UART2, UART3
} uart_device_t;

typedef enum uart_parity {
	NONE, EVEN, ODD, FORCED0, FORCED1
} uart_parity_t ;

typedef enum uart_stop_bits {
	ONE, TWO
} uart_stop_bits_t;

typedef enum uart_data_bits {
	FIVE, SIX, SEVEN, EIGHT,
} uart_data_bits_t;

/**
 * enables that uart device
 *
 * @param device to enable (UART0|UART1|UART2|UART3)
 */
void uart_init(uart_device_t device);

/**
 * Set the desired baudrate of the device
 *
 * @param device to set baudrate (UART0|UART1|UART2|UART3)
 * @param baud baudrate to set ex: 115200
 */
void uart_set_baud(uart_device_t device, uint32_t baud);

/**
 * Set the parity mode of the uart device
 *
 * @param device to set the parity mode (UART0|UART1|UART2|UART3)
 * @param parity (NONE|ODD|EVEN|FORCED1|FORCED0)
 */
void uart_set_parity(uart_device_t device, uart_parity_t parity);

/**
 * Sets how many data bits are used
 *
 * @param device to set the amount of data bits of (UART0|UART1|UART2|UART3)
 * @param bits (FIVE|SIX|SEVEN|EIGHT)
 */
void uart_set_data_bits(uart_device_t device, uart_data_bits_t bits);

/**
 * Returns the baudrate of the device
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return ex: 115200
 */
uint8_t uart_get_baud(uart_device_t device);

/**
 * Returns the parity mode of the device
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return (NONE|ODD|EVEN|FORCED1|FORCE0)
 */
uart_parity_t uart_get_parity(uart_device_t device);

/**
 * Returns the amount of data bits are used
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return (FIVE|SIX|SEVEN|EIGHT)
 */
uart_data_bits_t uart_get_data_bits(uart_device_t device);

/**
 * Returns if there is data to ready be read
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return  if readable (bool)
 */
uint8_t uart_is_readable(uart_device_t device);

/**
 * Returns if you can write safely to the device without overwriting
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return if writable (bool)
 */
uint8_t uart_is_writable(uart_device_t device);

/**
 * Read a character, will block until one is available
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return the read char
 */
char uart_getc(uart_device_t device);

/**
 * Writes a character, will block until writable
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @param c character to write
 */
void uart_putc(uart_device_t device, char c);

#endif /* UART_H_ */
