/*
 * distance_sensor.c
 *
 *  Created on: 25-okt.-2017
 *      Author: robin
 */

#include "distance_sensor.h"

void distance_sensor_init() {
	GPIO_init(21);
	GPIO_init(10);
	set_direction(10, 0);
}
