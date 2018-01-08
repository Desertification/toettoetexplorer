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
extern const uint8_t TRIGPIN;
extern const uint8_t ECHOPIN;

/**
 * Initialises the distance sensor
 */
void distance_sensor_init();

/**
 * returns the current distance measured by the sensor
 *
 * @return current distance (cm)
 */
uint16_t distance_sensor_get_distance_cm();


#endif /* DISTANCE_SENSOR_H_ */
