/*
 * uart.c
 *
 *  Created on: Nov 8, 2017
 *      Author: thoma
 */

#include "uart.h"
#include "clock.h"
#include "math.h"

// UART0	tx	p0.2		rx	p0.3		pin	USBTX, USBRX
// UART1	tx	p0.15		rx	p0.16		pin	p13, p14
// UART2	tx	p0.10		rx	p0.11		pin	p28, p27
// UART3	tx	p0.0		rx	p0.1		pin	p9, p10

static const uint32_t pconp_offset[] = { // power control register offset to enable uart
		3, 4, 24,25
};

static volatile uint32_t *pinmode_lut_tx[] = { // correct pinmode register for that uart tx device
		&LPC_PINCON->PINMODE0 ,&LPC_PINCON->PINMODE0,
		&LPC_PINCON->PINMODE0, &LPC_PINCON->PINMODE0
};

static const uint8_t pinmode_lut_tx_offset[] = {// starting index of pinmode register for uart tx
		4, 30, 20, 0
};

static volatile uint32_t *pinmode_lut_rx[] = {// correct pinmode register for that uart tx device
		&LPC_PINCON->PINMODE0 ,&LPC_PINCON->PINMODE1 ,
		&LPC_PINCON->PINMODE0, &LPC_PINCON->PINMODE0
};

static const uint8_t pinmode_lut_rx_offset[] = { // starting index of pinmode register for uart rx
		6, 0, 22, 1
};

static const uint8_t PINMODE_NONE = 2;

static volatile uint32_t *pinsel_lut_tx[] = { // correct pinmode register for that uart tx device
		&LPC_PINCON->PINSEL0 ,&LPC_PINCON->PINSEL0,
		&LPC_PINCON->PINSEL0, &LPC_PINCON->PINSEL0
};

static const uint8_t pinsel_lut_tx_offset[] = {// starting index of pinmode register for uart tx
		4, 30, 20, 0
};

static volatile uint32_t *pinsel_lut_rx[] = {// correct pinmode register for that uart tx device
		&LPC_PINCON->PINSEL0 ,&LPC_PINCON->PINSEL1 ,
		&LPC_PINCON->PINSEL0, &LPC_PINCON->PINSEL0
};

static const uint8_t pinsel_lut_rx_offset[] = { // starting index of pinmode register for uart rx
		6, 0, 22, 1
};

static const uint8_t pinsel_lut_func[] = { // function value for uart mode
		1, 1, 1, 2
};

static const volatile uint8_t *rbr[] = {
		&LPC_UART0->RBR, &LPC_UART1->RBR, &LPC_UART2->RBR, &LPC_UART3->RBR,
};

static volatile uint8_t *thr[] = {
		&LPC_UART0->THR, &LPC_UART1->THR, &LPC_UART2->THR, &LPC_UART3->THR,
};

static const volatile uint8_t *lsr[] = {
		&LPC_UART0->LSR, &LPC_UART1->LSR, &LPC_UART2->LSR, &LPC_UART3->LSR,
};

static volatile uint8_t *lcr[] = {
		&LPC_UART0->LCR, &LPC_UART1->LCR, &LPC_UART2->LCR, &LPC_UART3->LCR,
};

static volatile uint32_t *peripheral_clock[] = {
		&LPC_SC->PCLKSEL0, &LPC_SC->PCLKSEL0, &LPC_SC->PCLKSEL1, &LPC_SC->PCLKSEL1,
};

static const uint8_t peripherall_clock_offset[] = { //starting indexes for the preipherall clock registers for uart
		6, 8, 16, 18
};

static float fractional_divider_setting[] = {
		1.000, 1.067, 1.071, 1.077, 1.083, 1.091, 1.100, 1.111, 1.125, 1.133, 1.143, 1.154, 1.167, 1.182, 1.200, 1.214, 1.222, 1.231,
		1.250, 1.267, 1.273, 1.286, 1.300, 1.308, 1.333, 1.357, 1.364, 1.375, 1.385, 1.400, 1.417, 1.429, 1.444, 1.455, 1.462, 1.467,
		1.500, 1.533, 1.538, 1.545, 1.556, 1.571, 1.583, 1.600, 1.615, 1.625, 1.636, 1.643, 1.667, 1.692, 1.700, 1.714, 1.727, 1.733,
		1.750, 1.769, 1.778, 1.786, 1.800, 1.818, 1.833, 1.846, 1.857, 1.867, 1.875, 1.889, 1.900, 1.909, 1.917, 1.923, 1.929, 1.933
};


static uint8_t fractional_divider_divaddval[] = {
		0, 1, 1, 1, 1, 1, 1, 1, 1, 2,
		1, 2, 1, 2, 1, 3, 2, 3, 1, 4,
	    3, 2, 3, 4, 1, 5, 4, 3, 5, 2,
		5, 3, 4, 5, 6, 7, 1, 8, 7, 6,
		5, 4, 7, 3, 8, 5, 7, 9, 2, 9,
		7, 5, 8, 11, 3, 10, 7, 11, 4, 9,
		5, 11, 6, 13, 7, 8, 9, 10, 11, 12,
		13, 14
};

static uint8_t fractional_divider_mulval[] = {
		1, 15, 14, 13, 12, 11, 10, 9, 8, 15,
		7, 13, 6, 11, 5, 14, 9, 13, 4, 15,
		11, 7, 10, 13, 3, 14, 11, 8, 13, 5,
		12, 7, 9, 11, 13, 15, 2, 15, 13, 11,
		9, 7, 12, 5, 13, 8, 11, 14, 3, 13,
		10, 7, 11, 15, 4, 13, 9, 14, 5, 11,
		6, 13, 7, 15, 8, 9, 10, 11, 12, 13,
		14, 15
};

static const uint8_t FRACTIONAL_DIVIDER_LENGTH = sizeof(fractional_divider_mulval)/sizeof(fractional_divider_mulval[0]);

static volatile uint8_t *dll[] = {
		&LPC_UART0->DLL, &LPC_UART1->DLL, &LPC_UART2->DLL, &LPC_UART3->DLL
};

static volatile uint8_t *dlm[] = {
		&LPC_UART0->DLM, &LPC_UART1->DLM, &LPC_UART2->DLM, &LPC_UART3->DLM
};

static volatile uint8_t *fdr[] = {
		&LPC_UART0->FDR, (volatile uint8_t *) &LPC_UART1->FDR, &LPC_UART2->FDR, &LPC_UART3->FDR
};

//====================== forward declarations ======================//

static uint8_t device_to_index(uart_device_t device);

static uint8_t is_readable(uint8_t index);
static uint8_t is_writable(uint8_t index);

static void config_power_ctrl_uart(uint8_t index);
static void config_pinmode_none(uint8_t index);
static void config_pinsel_uart(uint8_t index);
static void config_pclock_uart(uint8_t index);

static uint32_t get_peripheral_clock(uint8_t index);

static uint8_t isInteger(double value);

static void calc_fractional_divider(uint32_t baud, uint32_t pclk, double * const frestPtr, double * const dlestPtr);
static uint8_t find_closest_fractional_divider_index(float frest);

static void enable_divisor_latch_access(uint8_t index);
static void disable_divisor_latch_access(uint8_t index);

static void config_divisor_latches(uint8_t index, uint16_t dl, uint8_t divaddval, uint8_t mulval);

//====================== global functions ==========================//

/**
 * enables that uart device
 *
 * @param device to enable (UART0|UART1|UART2|UART3)
 */
void uart_init(uart_device_t device){
	uint8_t index = device_to_index(device);

	config_power_ctrl_uart(index);
	config_pinmode_none(index);
	config_pinsel_uart(index);
	config_pclock_uart(index);
}

/**
 * Set the desired baudrate of the device
 *
 * @param device to set baudrate (UART0|UART1|UART2|UART3)
 * @param baud baudrate to set ex: 115200
 */
void uart_set_baud(uart_device_t device, uint32_t baud){
	// todo limit baud rate to range of values
	uint8_t index = device_to_index(device);
	uint32_t pclk = get_peripheral_clock(index);

	uint8_t divaddval;
	uint8_t mulval;
	double frest;
	double dlest = pclk/(16.0*baud);

	if (isInteger(dlest)){
		divaddval = 0;
		mulval = 1;
	} else {
		frest = 1.5;
		double * const frestPtr = &frest;
		double * const dlestPtr = &dlest;
		calc_fractional_divider(baud, pclk, frestPtr, dlestPtr);
		uint8_t fd_index = find_closest_fractional_divider_index(frest);
		divaddval = fractional_divider_divaddval[fd_index];
		mulval = fractional_divider_mulval[fd_index];
	}

	config_divisor_latches(index, (uint16_t) dlest, divaddval, mulval);
}

/**
 * Returns the baudrate of the device
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return ex: 115200
 */
uint8_t uart_get_baud(uart_device_t device){
	uint8_t index = device_to_index(device);
	//todo
}

/**
 * Set the parity mode of the uart device
 *
 * @param device to set the parity mode (UART0|UART1|UART2|UART3)
 * @param parity (NONE|ODD|EVEN|FORCED1|FORCED0)
 */
void uart_set_parity(uart_device_t device, uart_parity_t parity){
	uint8_t index = device_to_index(device);

	switch (parity) {
		case NONE:
			*lcr[index] &= ~(1 << 3); //disable parity
			break;
		default:
			*lcr[index] |= (1 << 3); //enable parity
			break;
	}

	*lcr[index] &= ~(3 << 4); //clear parity select register

	switch (parity) {
		case EVEN:
			*lcr[index] |= (1 << 4);
			break;
		case FORCED1:
			*lcr[index] |= (2 << 4);
			break;
		case FORCED0:
			*lcr[index] |= (3 << 4);
			break;
		case ODD:
		default:
			*lcr[index] |= (0 << 4);
			break;
	}
}

/**
 * Sets how many data bits are used
 *
 * @param device to set the amount of data bits of (UART0|UART1|UART2|UART3)
 * @param bits (FIVE|SIX|SEVEN|EIGHT)
 */
void uart_set_data_bits(uart_device_t device, uart_data_bits_t bits){
	uint8_t index = device_to_index(device);

	*lcr[index] &= ~3; //clear word length register

	switch (bits) {
		case FIVE:
			*lcr[index] |= 0;
			break;
		case SIX:
			*lcr[index] |= 1;
			break;
		case SEVEN:
			*lcr[index] |= 2;
			break;
		case EIGHT:
		default:
			*lcr[index] |= 3;
			break;
	}
}

/**
 * Returns the parity mode of the device
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return (NONE|ODD|EVEN|FORCED1|FORCE0)
 */
uart_parity_t uart_get_parity(uart_device_t device){
	uint8_t index = device_to_index(device);

	if ((*lcr[index] & (1 << 3)) == 0){
		return NONE;
	}

	uint8_t parity_select = (*lcr[index] & (3 << 4) >> 4);
	switch (parity_select) {
		case 1:
			return EVEN;
		case 2:
			return FORCED1;
		case 3:
			return FORCED0;
		case 0:
		default:
			return ODD;
			break;
	}
}

/**
 * Returns the amount of data bits are used
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return (FIVE|SIX|SEVEN|EIGHT)
 */
uart_data_bits_t uart_get_data_bits(uart_device_t device){
	uint8_t index = device_to_index(device);

	uint8_t word_length = (*lcr[index] & 3);
	switch (word_length){
		case 0:
			return FIVE;
		case 1:
			return SIX;
		case 2:
			return SEVEN;
		case 3:
		default:
			return EIGHT;
			break;
	}
}

/**
 * Returns if there is data to ready be read
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return  if readable (bool)
 */
uint8_t uart_is_readable(uart_device_t device){
	uint8_t index = device_to_index(device);
	return is_readable(index);
}

/**
 * Returns if you can write safely to the device without overwriting
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return if writable (bool)
 */
uint8_t uart_is_writable(uart_device_t device){
	uint8_t index = device_to_index(device);
	return is_writable(index);
}

/**
 * Read a character, will block until one is available
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @return the read char
 */
char uart_getc(uart_device_t device){
	uint8_t index = device_to_index(device);
	while(!is_readable(index));
	return *rbr[index];
}

/**
 * Writes a character, will block until writable
 *
 * @param device (UART0|UART1|UART2|UART3)
 * @param c character to write
 */
void uart_putc(uart_device_t device, char c){
	uint8_t index = device_to_index(device);
	while(!is_writable(index));
	*thr[index] = c;
}

//====================== static functions ==========================//

/**
 * convert device enum to index for easy luts
 */
static uint8_t device_to_index(uart_device_t device){
	switch (device) {
		case UART0:
			return 0;
		case UART1:
			return 1;
		case UART2:
			return 2;
		case UART3:
			return 3;
		default:
			return 0;
	}
}

/**
 * returns if there is data ready to be read
 */
uint8_t is_readable(uint8_t index){
	return !(*lsr[index] & 1) == 0; //receiver data ready
}

/**
 * returns if data can be written
 */
uint8_t is_writable(uint8_t index){
	return !(*lsr[index] & (1 << 6)) == 0; //transmitter empty
}

/**
 * enable power for uart
 */
static void config_power_ctrl_uart(uint8_t index){
	LPC_SC->PCONP |= (1 << pconp_offset[index]);
}

/**
 * disable pull-up/down
 */
static void config_pinmode_none(uint8_t index){
	//clear
	*pinmode_lut_tx[index] &= ~(3 << pinmode_lut_tx_offset[index]);
	*pinmode_lut_rx[index] &= ~(3 << pinmode_lut_rx_offset[index]);

	//set mode
	*pinmode_lut_tx[index] |= (PINMODE_NONE << pinmode_lut_tx_offset[index]);
	*pinmode_lut_rx[index] |= (PINMODE_NONE << pinmode_lut_rx_offset[index]);
}

/**
 * set pin function to uart
 */
static void config_pinsel_uart(uint8_t index){
	//clear
	*pinsel_lut_tx[index] &= ~(3 << pinsel_lut_tx_offset[index]);
	*pinsel_lut_rx[index] &= ~(3 << pinsel_lut_rx_offset[index]);

	//set function
	*pinsel_lut_tx[index] |= (pinsel_lut_func[index] << pinsel_lut_tx_offset[index]);
	*pinsel_lut_rx[index] |= (pinsel_lut_func[index] << pinsel_lut_rx_offset[index]);
}

/**
 * set peripheral clock to clk/4
 */
static void config_pclock_uart(uint8_t index){
	*peripheral_clock[index] &= ~(3 << peripherall_clock_offset[index]);
}

/**
 * checks if the value can be converted to int without loss
 */
static uint8_t isInteger(double value){
	uint32_t value_int = (int) value;
	return value_int == value;
}

/**
 * returns the peripheral clock for uart
 */
static uint32_t get_peripheral_clock(uint8_t index){
	uint8_t offset = peripherall_clock_offset[index];
	uint8_t config = (*peripheral_clock[index] & (3 << offset)) >> offset;

	uint8_t divider;
	switch (config) {
		case 1:
			divider = 1;
			break;
		case 2:
			divider = 2;
			break;
		case 3:
			divider = 8;
			break;
		case 0:
		default:
			divider = 4;
			break;
	}
	return CLOCKFREQ / divider;
}

/**
 * uart fractional divider algorithm (figure 47 chapter 14 p323)
 */
void calc_fractional_divider(uint32_t baud, uint32_t pclk, double * const frestPtr, double * const dlestPtr){
	double dlest = *dlestPtr;
	double frest = *frestPtr;
	double frest_copy = frest;

	if (frest < 1.1 || frest > 1.9) {
		return; // baudrate is not possible
	}

	dlest = (int) (pclk/(16*baud*frest));
	frest = pclk/(16*baud*dlest);

	*dlestPtr = dlest;

	if (frest < 1.1){
		*frestPtr = frest_copy + 0.1;
		calc_fractional_divider(baud, pclk, frestPtr, dlestPtr);
	}else if (frest > 1.9) {
		*frestPtr = frest_copy - 0.1;
		calc_fractional_divider(baud, pclk, frestPtr, dlestPtr);
	} else {
		*frestPtr = frest;
	}
}

/**
 * uart fractional divider algorithm (figure 47 chapter 14 p323)
 * find the nearest setting (table 287 chapter 14 p324)
 */
uint8_t find_closest_fractional_divider_index(float frest){
	float smallest_difference = 2.0; // difference is max 1, so 2 is a good starting value
	uint8_t index_closest_divider = 0;
	float difference;
	for (uint8_t i = 0; i < FRACTIONAL_DIVIDER_LENGTH; ++i) {
		difference = fabsf(frest - fractional_divider_setting[i]);
		if (difference < smallest_difference) {
			smallest_difference = difference;
			index_closest_divider = i;
		}
	}
	return index_closest_divider;
}

/**
 * Enable access to the divisor latches (DLAB)
 */
static void enable_divisor_latch_access(uint8_t index){
	*lcr[index] |= (1 << 7);
}

/**
 * Disable access to the divisor latches (DLAB)
 */
static void disable_divisor_latch_access(uint8_t index){
	*lcr[index] &= ~(1 << 7);
}

/**
 * Configure the dlm, dll and fdr registers
 */
static void config_divisor_latches(uint8_t index ,uint16_t dl, uint8_t divaddval, uint8_t mulval){
	enable_divisor_latch_access(index);

	*dlm[index] = (uint8_t) dl >> 8;
	*dll[index] = (uint8_t) dl & 0xff;
	*fdr[index] = (mulval << 4) | divaddval;

	disable_divisor_latch_access(index);
}
