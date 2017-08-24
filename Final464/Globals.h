/*
 * Authors: Lakshay Duggal, Tim Oorschot
 * globals.h - Initialises the struct and variables that are to be used by main.c
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <stdint.h>

// Structure definition
struct height_s {
	float current;
	float previous;
};

// Variables for interrupts
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
