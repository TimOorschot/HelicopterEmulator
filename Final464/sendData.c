/*
 * Authors: Lakshay Duggal, Tim Oorschot
 * sendData.c - takes calculated values and sends them using SSI to the DAC.
 * 			  - also generates the quadrature signal
 */
#include <stdbool.h>
#include "globals.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"

void generateQuad(struct height_s yaw)
{
	/*
	 * Generates a quadrature signal based the current and previous yaw.
	 * Checks wether the current angle is higher/lower than the next slot.
	 * If the slot is higher than the target angle is incremented and a pulse is sent on two pins based
	 * on which direction the helicopter is moving.
	 */

	int steveJobs = 0; 							// Used to count till the desired pulse width is reached
	int width = 10000; 							// Desired pulse width
	int threeSixtyDeg = 360000; 				// Used to add to relative angle
	int yawIntCurrent = (yaw.current * 1000); 	// Current angle multipled by 1000 to keep accuracy after converting to int
	static int yawIntPrevious = 0; 				// Previous yaw value. Used to keep track of direction
	int Increment = 360000 / 112; 				// Increments relAngle after the helicopter passes the next slot
	static int relAngle = 0;					// Angle of next slot
	static int counter = 0; 					// Keeps count of how many rotations have been completed by the helicopter.

	int yawInternal = counter * threeSixtyDeg + yawIntCurrent; // Add (number of rotations) x (360 degrees)

	if ((yawIntPrevious > 0) && (yawIntCurrent == 0)){
		// Increment counter if a rotation has been completed
		counter++;
	}
	else if ((yawIntPrevious < 0) && (yawIntCurrent == 0)){
		counter--;
	}

	yawIntPrevious = yawIntCurrent; // Store previous value

	if (yawInternal > relAngle) {
		// If yawInternal is greater than the angle of the next slot, then increment next slot angle.
		while(yawInternal > relAngle){
			// Keep incrementing until the target is greater than current angle.
			relAngle = relAngle + Increment;
		}

		// Set pin High
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xff);
		steveJobs = 0;
		while (steveJobs < width){
			// Wait until desired pulse width
			steveJobs++;
		}

		// Delay send pin.
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xff);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}

		// Switch pins low in corrent order
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}
	}
	// If the helicopter is spinnig in opposite direction
	else if (yawInternal < relAngle - Increment) {
		while(yawInternal < relAngle - Increment){
		relAngle = relAngle - Increment;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xff);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xff);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);
		steveJobs = 0;
		while (steveJobs < width){
			steveJobs++;
		}
	}
}

struct height_s referncePoint(struct height_s yaw)
{
	// Sets reference pin high when heli passes 360 degrees

	int i = 0;
	int width = 10000;

	if (yaw.current == 0){
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0xff);
		while (i < width){
			i++;
		}
	}
	else{
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x00);
	}
	yaw.current = yaw.current * 180 / pi;
	yaw.previous = yaw.previous * 180 / pi;

	// Return as degrees
	return yaw;
}


void sendHeight(struct height_s height)
{
	// Sends the current height to the DAC via SPI
	int voltageLow = 16383;							// Voltage value when the helicopter is at the lowest point
	int voltageHigh = 14336;						// Voltage value when the helicopter is at the highest point
	int voltageDiff = voltageLow - voltageHigh;		// Difference in high and low voltage

	float heightLow = 46.5203;						// Lowet height
	float heightHigh = 153.4797;					// Highest heigh
	float heightDiff = heightHigh - heightLow;		// Difference height

	float heightP = ((height.current - heightLow)/heightDiff)*100; 	// Percentage height

	float DACHeight = voltageLow - (heightP*voltageDiff)/100;		// Value to be sent to DAC

	uint32_t DATA = DACHeight;						// Converting to int
	SSIDataPut(SSI0_BASE, DATA);					// Send to DAC

}
