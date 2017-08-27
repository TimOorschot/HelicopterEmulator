/*
 * Authors: Lakshay Duggal, Tim Oorschot
 *
 * setup.c - Initialises and configures peripherals
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "interrupts.h"
#include "driverlib/ssi.h"

void EnablePeriph (void)
{
	/*
	 * Enables and sets up peripherals for timers and GPIO
	 */

	// Sets main clock frequency to 12.5 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_16 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
            SYSCTL_OSC_MAIN);

	// Enable port F and B for PWM input and output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Set pins as inputs and putputs
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1); 							// Main PWM in
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0); 							// Tail PWM in
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);	// Quadrature encoder out

	// Setup timer for main PWM input
	// Code used from example: timers
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
	TimerEnable(TIMER0_BASE, TIMER_A);

	// Setup timer for tail PWM input
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet());
	TimerEnable(TIMER1_BASE, TIMER_A);
}

void ConfigureUART(void)
{
	/*
	 * Sets up UART
	 * Code taken from example: freeRTOS
	 */

    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 9600, 16000000);
}


void ConfigureInts(void)
{
	/*
	 * Configures interrupts
	 */

	// Using two seprate ports means that he interrupts do not interferes
	GPIOIntRegister(GPIO_PORTF_BASE, mainPWMInt);					// Main PWM interrupt
	GPIOIntRegister(GPIO_PORTB_BASE, tailPWMInt);					// Tail PWM interrupt
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_BOTH_EDGES);	// Use both edges as the input
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_BOTH_EDGES);	// 	signal is a PWM

	// Enable interrupts
	IntMasterEnable();
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0);

}

uint32_t spiClock = 5000000;

void initSPIPins(void)
{
	/*
	 * Setting up SPI to be used with DAC
	 */

	// Enable SSI peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK); // Pin PA2 - clock
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);	// Pin PA3 - chip select
	GPIOPinConfigure(GPIO_PA5_SSI0TX);	// Pin PA5 - data

	// Setup pins
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
	SSIIntClear(SSI0_BASE, SSI_TXEOT);

	// Configure and setup
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, spiClock, 16);
	SSIEnable(SSI0_BASE);
}

void __init__(void)
{
	/*
	 * Function to initialise peripherals in the correct order
	 */
	EnablePeriph();
	ConfigureUART();
	ConfigureInts();
	initSPIPins();
}
