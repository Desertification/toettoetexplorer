/*
 * init.c
 *
 *  Created on: Dec 13, 2017
 *      Author: thoma
 */

#include "init.h"
#include "pwm.h"

void init_hardware(){
	pwm_set_frequency(200); //frequency is shared by all the pwm devices
}