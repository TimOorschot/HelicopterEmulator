//*****************************************************************************
//
// project.c - Simple project to use as a starting point for more complex
//             projects.
//
// Copyright (c) 2013-2016 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.1.3.156 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.c"

#include "pwm_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Simple Project (project)</h1>
//!
//! A very simple example that can be used as a starting point for more complex
//! projects.  Most notably, this project is fully TI BSD licensed, so any and
//! all of the code (including the startup code) can be used as allowed by that
//! license.
//!
//! The provided code simply toggles a GPIO using the Tiva Peripheral Driver
//! Library.
//
//*****************************************************************************

//*****************************************************************************
//
// GLOBAL DEFINITIONS
//
//*****************************************************************************

uint32_t Direction;
uint32_t Altitude[1];

#define RED_LED   GPIO_PIN_1

//*****************************************************************************
//
// MUTEX
//
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;


//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// ADC
//
/*****************************************************************************
void
ConfigureADC(void) {


	//ENABLE ADC0
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	while(!(ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)));

	//ENABLE ADC PIN
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	ROM_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);

	//ADC CONFIG
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
	                              ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC0_BASE, 3);
	ROM_ADCIntClear(ADC0_BASE, 3);
}

void
vReadADC(void) {

	//Trigger ADC
	ROM_ADCProcessorTrigger(ADC0_BASE, 3);

    // Wait for conversion to be completed.
	while(!ROM_ADCIntStatus(ADC0_BASE, 3, false))
	{
	}

	// Clear the ADC interrupt flag.
	ROM_ADCIntClear(ADC0_BASE, 3);

	// Read ADC Value.
	ROM_ADCSequenceDataGet(ADC0_BASE, 3, Altitude);

}
*/
//*****************************************************************************
//
// SSI
//
//*****************************************************************************

void
ConfigureSSI (void) {
	// Enable the SSI0 peripheral
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);
	//
	// Wait for the SSI0 module to be ready.
	//
	while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI3))
	{
	}
	//
	// Configure the SSI.
	//
	ROM_SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 2000000, 8);
	//
	// Enable the SSI module.
	//
	ROM_SSIEnable(SSI3_BASE);
	//
}

void
sendSSI(char *pcChars) {

	int32_t i32Idx = 0;

	// Send some data.
	while(pcChars[i32Idx])
	{
	SSIDataPut(SSI3_BASE, pcChars[i32Idx]);
	i32Idx++;
	}

}

//*****************************************************************************
//
// PWM
//
/*****************************************************************************

void
ConfigurePWM(void) {
	//
	// Enable the PWM0 peripheral
	//

	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	//
	// Wait for the PWM0 module to be ready.
	//
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
	{
	}

	//Pin Configure
	GPIOPinConfigure(GPIO_PC5_M0PWM7);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

	// Configure the PWM generator for count down mode with immediate updates
	// to the parameters.
	//
	PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	//
	// Set the period. For a 50 KHz frequency, the period = 1/50,000, or 20
	// microseconds. For a 20 MHz clock, this translates to 400 clock ticks.
	// Use this value to set the period.
	//
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, 320);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 320);
	//
	// Set the pulse width of PWM0 for a 25% duty cycle.
	//
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 100);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 100);
	//
	// Start the timers in generator 0.
	//
	PWMGenEnable(PWM0_BASE, PWM_GEN_3);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	//
	// Enable the outputs.
	//
	PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);
}

//NEED TO UPDATE
void
pwmSet(void) {

	// Set the period. For a 50 KHz frequency, the period = 1/50,000, or 20
	// microseconds. For a 20 MHz clock, this translates to 400 clock ticks.
	// Use this value to set the period.
	//
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, 320);
	//
	// Set the pulse width of PWM0 for a 25% duty cycle.
	//
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 100);
	//
	// Start the timers in generator 0.
	//
	PWMGenEnable(PWM0_BASE, PWM_GEN_3);
	//
	// Enable the outputs.
	//
	PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);
}

*/

//*****************************************************************************
//
// INTERRUPTS
//
/*****************************************************************************

void
IntGPIOb(void)
{
	if (Direction != ROM_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1))
		Display();
	Direction = ROM_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);
}

void
ConfigureInts(void) {

	GPIOIntRegister(GPIO_PORTB_BASE, IntGPIOb);

	ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
	ROM_GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);

	ROM_IntMasterEnable();

	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0);
}

*/

//*****************************************************************************
//
// INFORMATION DISPLAY
//
//*****************************************************************************
void
Display(void) {
	// Display the AIN0 (PE4) digital value on the console.
	printf("Altitude = %4d\r \n", Altitude[0]);
	printf(Direction && 0x02 ? "Direction = CCW\n" : "Direction = CW\n" );
}


//*****************************************************************************
//
// MAIN
//
//*****************************************************************************


int main (void) {
	 // Set the clocking to run at 50 MHz from the PLL.
	    //
	    cgfcfbf(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
	                       SYSCTL_OSC_MAIN);

	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


	    //
	    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
	    //
	    ConfigureUART();

	    //
	    // Print demo introduction.
	    //
	    puts("You're good");
	    fflush(stdout);
	    printf( "You're good! \n");

	    //
	    // Create a mutex to guard the UART.
	    //
	    g_pUARTSemaphore = xSemaphoreCreateMutex();

	    //
	    // Create the PWM task.
	    //
	    if(PWMTaskInit() != 0)
	    {

	    	while(1)
	    	{
	    	}
	    }

	    //
	    // Create the switch task.
	    //
	    if(SwitchTaskInit() != 0)
	    {

	        while(1)
	        {
	        }
	    }

	    //
	    // Start the scheduler.  This should not return.
	    //
	    vTaskStartScheduler();

	    //
	    // In case the scheduler returns for some reason, print an error and loop
	    // forever.
	    //

	    while(1)
	    {
	    }
}


	 /*
    while(1)
    {
    	pwmSet();

    	//Get Height Value
    	vReadADC();
    	//Display();

    	if (displayDelay == 1000) {
    		Display();
    		displayDelay = 0;
    	}
    	displayDelay++;

    }

    */

