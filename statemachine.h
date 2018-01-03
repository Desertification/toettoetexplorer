/*
 * statemachine.h
 *
 *  Created on: Dec 19, 2017
 *      Author: thoma
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

typedef enum state {
	INIT, DRIVE, LOOK, STUCK, TLEFT, TRIGHT
} state_t;


/**
 * Runs the statemachine. is blocking.
 *
 * @param initial_state initial state to start in
 */
void statemachine_run(state_t initial_state);

/**
 * Sets the next state of the statemachine.
 *
 * @param state next state to run
 */
void statemachine_set(state_t state);

/**
 * Gets the current state of the statemachine
 *
 * @return current state
 */
state_t statemachine_get();

#endif /* STATEMACHINE_H_ */
