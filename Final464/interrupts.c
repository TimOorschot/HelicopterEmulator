/*
 * interrupts.c
 *
 *  Created on: Aug 24, 2017
 *      Author: ldu30
 */
#include <stdbool.h>
#include "globals.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

void mainPWMInt(void)
{
	uint32_t temp = TimerValueGet(TIMER0_BASE, TIMER_A);
	int pinHigh = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1);

	if (pinHigh) {
		prevRisingMain = risingMain;
		risingMain = temp;

		PWM.previous = PWM.current;
		PWM.current = (abs(prevRisingMain - fallingMain)*100)/(abs(prevRisingMain - risingMain));

	}
	else {
		fallingMain = temp;

	}
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
}

void tailPWMInt (void)
{
	uint32_t temp = TimerValueGet(TIMER1_BASE, TIMER_A);
	int pinHigh = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);

	if (pinHigh) {
		prevRisingTail = risingTail;
		risingTail = temp;

		PWMTail.previous = PWMTail.current;
		PWMTail.current = (abs(prevRisingTail - fallingTail)*100)/(abs(prevRisingTail - risingTail));

	}
	else {
		fallingTail = temp;

	}
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);
}
