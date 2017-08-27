/*
 * Authors: Lakshay Duggal, Tim Oorschot
 * interrupt.c - Defines the ISR for both PWMs
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
	/*
	 * Main PWM
	 */

	// Check timer at interrupt
	uint32_t temp = TimerValueGet(TIMER0_BASE, TIMER_A);

	// Read pin
	int pinHigh = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1);

	// If pin is high, interrupted at rising edge
	if (pinHigh) {

		// Set previous timer value
		prevRisingMain = risingMain;

		// Set new timer value
		risingMain = temp;

		// Set previous PWM value
		PWM.previous = PWM.current;

		// Calculate duty cycle (%)
		PWM.current = (abs(prevRisingMain - fallingMain)*100)/(abs(prevRisingMain - risingMain));

	}
	else {

		// If not high then the falling edge, store timer value for event.
		fallingMain = temp;

	}

	// Clear interrupt
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);
}

void tailPWMInt (void)
{
	/*
	 * Tail PWM
	 */

	// Check timer at interrupt
	uint32_t temp = TimerValueGet(TIMER1_BASE, TIMER_A);

	// Read pin
	int pinHigh = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);

	// If pin is high, interrupted at rising edge
	if (pinHigh) {

		// Set previous timer value
		prevRisingMain = risingMain;

		// Set new timer value
		risingMain = temp;

		// Set previous PWM value
		PWMTail.previous = PWMTail.current;

		// Calculate duty cycle (%)
		PWMTail.current = (abs(prevRisingTail - fallingTail)*100)/(abs(prevRisingTail - risingTail));

	}
	else {

		// If not high then the falling edge, store timer value for event.
		fallingTail = temp;

	}

	// Clear interrupt
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);
}
