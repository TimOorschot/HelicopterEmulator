/*
 * Globals.h
 *
 *  Created on: Aug 20, 2017
 *      Author: ldu30
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <stdint.h>

struct height_s { //structure definition
	float current;
	float previous;
};

uint32_t risingMain;
uint32_t fallingMain;
uint32_t prevRisingMain;

uint32_t risingTail;
uint32_t fallingTail;
uint32_t prevRisingTail;

struct height_s PWM;
struct height_s PWMTail;

#define pi 3.141592654f;

#endif /* GLOBALS_H_ */
