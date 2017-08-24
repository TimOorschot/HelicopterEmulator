/*
 * sendData.c
 *
 *  Created on: Aug 24, 2017
 *      Author: ldu30
 */
#include <stdbool.h>
#include "globals.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"

void generateQuad(struct height_s yaw)
{
	int steveJobs = 0;
	int threeSixtyDeg = 360000;
	int yawIntCurrent = (yaw.current * 1000);
	static int yawIntPrevious = 0;
	int Increment = 360000 / 112;
	static int relAngle = 0;
	static int counter = 0;

	int yawInternal = counter * threeSixtyDeg + yawIntCurrent;

	if ((yawIntPrevious > 0) && (yawIntCurrent == 0)){
		counter++;
	}
	else if ((yawIntPrevious < 0) && (yawIntCurrent == 0)){
		counter--;
	}


	yawIntPrevious = yawIntCurrent;

	if (yawInternal > relAngle) {
		while(yawInternal > relAngle){
		relAngle = relAngle + Increment;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xff);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xff);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}
	}
	else if (yawInternal < relAngle - Increment) {
		while(yawInternal < relAngle - Increment){
		relAngle = relAngle - Increment;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0xff);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0xff);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}

		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);
		steveJobs = 0;
		while (steveJobs < 10000){
			steveJobs++;
		}
	}
}

struct height_s referncePoint(struct height_s yaw){

	int phillipJonathanBones = 0;

	if (yaw.current == 0){
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0xff);
		while (phillipJonathanBones < 10000){
			phillipJonathanBones++;
		}
	}
	else{
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x00);
	}
	yaw.current = yaw.current * 180 / pi;
	yaw.previous = yaw.previous * 180 / pi;

	return yaw;
}


void sendHeight(struct height_s height) {
	int voltageLow = 16383;
	int voltageHigh = 14336;
	int voltageDiff = voltageLow - voltageHigh;

	float heightLow = 46.5203;
	float heightHigh = 153.4797;
	float heightDiff = heightHigh - heightLow;

	float heightP = ((height.current - heightLow)/heightDiff)*100;

	float DACHeight = voltageLow - (heightP*voltageDiff)/100;

	uint32_t DATA = DACHeight;
	SSIDataPut(SSI0_BASE, DATA);

	//UARTprintf("P %d, DAC %d \n", test1, DATA);
}
