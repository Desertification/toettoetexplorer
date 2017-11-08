/*
 * distance_sensor.h
 *
 *  Created on: 25-okt.-2017
 *      Author: robin
 */

#ifndef DISTANCE_SENSOR_H_
#define DISTANCE_SENSOR_H_

#include "gpio.h"

//pinnames
const uint8_t TRIGPIN = 21;
const uint8_t ECHOPIN = 10;

uint16_t distance;

void distance_sensor_init();
uint16_t distance_sensor_get_distance_cm();

#endif /* DISTANCE_SENSOR_H_ */
