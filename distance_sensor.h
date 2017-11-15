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

void distance_sensor_init();
void distance_sensor_update_distance_cm();
uint16_t distance_sensor_get_distance_cm();

void send_pulse();
void wait_for_echo();
void receive_echo();

#endif /* DISTANCE_SENSOR_H_ */
