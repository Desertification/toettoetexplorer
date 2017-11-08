/*
===============================================================================
 Name        : toettoetexplorer.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#include "motor.h"

int main(void) {
	motor_init();
	motor_drive(BACKWARDS);
	motor_set_speed(70);
}
