/*
 * Authors: Lakshay Duggal, Tim Oorschot
 * main.c - Combines functions to calculate and display functions of the helicopter.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils/uartstdio.h"
#include "Globals.h"
#include "HeightandTail.h"
#include "setup.h"
#include "interrupts.h"
#include "sendData.h"


int main() {

	// Initialise peripherials
	__init__();

	UARTprintf("test \n"); // Test UART

	// Set initial values for PWM
	PWM.current = 0;
	PWM.previous = 0;

	PWMTail.current = 0;
	PWMTail.previous = 0;

	// Creates structs to be used in functions.
	struct height_s height;
	height.current = 0;
	height.previous = 0;

	struct height_s yaw;
	yaw.current = 0;
	yaw.previous = 0;

	struct height_s yawDeg;
	yawDeg.current = 0;
	yawDeg.previous = 0;

	int i = 0;

	// Initialise ints for UART
	int heightInt;
	int PWMInt;
	int yawDegInt;
	int PWMTailInt;

	while(1) {

		// Slow down calculation speed
		if (i>12500) {

			// Calculate variables
			height = claculateHeight(PWM);

			sendHeight(height);

			yaw = calculateYaw(PWM, PWMTail);

			// Check if current angle is at or past the reference point.
			yawDeg = referncePoint(yaw); // Also converts to degrees
			generateQuad(yawDeg);

			// Convert to ints
			heightInt = height.current;
			PWMInt = PWM.current;
			yawDegInt  = yawDeg.current;
			PWMTailInt = PWMTail.current;

			// Print values using UART
			UARTprintf("height = %d (%d), yaw = %d (%d) \n", heightInt, PWMInt, yawDegInt, PWMTailInt);

			i = 0;
		}

		else {
			i++;
		}
	}
}

