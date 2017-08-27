//*****************************************************************************
//
// project.c - Main function for helicopter Controller
// Authors: Neeraj Patel, Jono Kapene, Lakshay Duggal, Tim Oorschot
// Date:	24/8/2017
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


#include "yaw_task.h"
#include "display_task.h"
#include "adc_task.h"
#include "pwm_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


//*****************************************************************************
//
// GLOBAL DEFINITIONS
//
//*****************************************************************************

//CURRENT VALUES
uint8_t g_Direction;		// Direction of Rotation
uint32_t g_Altitude[1];		// ADC altitude reading
uint32_t g_Yaw = 0;			// Yaw Value
int8_t g_dutyMain = 20;		// Duty Cycle for Main Rotor
int8_t g_dutyRotor;			// Duty Cycle for Tail Rotor


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




//*****************************************************************************
//
// MAIN
//
//*****************************************************************************


int main (void) {
	 // Set the clocking to run at 50 MHz from the PLL.
	    //
	    (SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
	                       SYSCTL_OSC_MAIN);

	    //Initialise all pins on board

	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


	    //
	    // Create the Yaw task.
	    //
	    if(YawTaskInit() != 0)
	    {

	    	while(1)
	    	{
	    	}
	     }

	    //
	   	// Create the Display task.
	   	//
	   	if(DisplayTaskInit() != 0)
	   	{

	   		while(1)
	   	    {
	   	    }
	   	}


	    //
	   	// Create the ADC task.
	   	//
	   	if(ADCTaskInit() != 0)
	   	{

	   	    while(1)
	   	    {
	   	    }
	   	}

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


