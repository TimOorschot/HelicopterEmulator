//*****************************************************************************
//
// pwm_task.c -  Initialises and describes pwm output behaviour for Helicopter Controller
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
#include "drivers/buttons.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "display_task.h"
#include "PWM_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************
//
// The stack size for the PWM Task..
//
//*****************************************************************************
#define PWMTASKSTACKSIZE        256         // Stack size in words
#define	DISPLAY 0x00000001

//*****************************************************************************
//
// The item size and queue size for the PWM message queue.
//
//*****************************************************************************
#define PWM_ITEM_SIZE           sizeof(uint8_t)
#define PWM_QUEUE_SIZE          5

//*****************************************************************************
//
// Default PWM toggle delay value. PWM toggling frequency is twice this number.
//
//*****************************************************************************
#define PWM_TOGGLE_DELAY        250


//*****************************************************************************
//
// Global Duty Cycles
//
//****************************************************************************

extern int8_t g_dutyMain, g_dutyRotor;



//*****************************************************************************
//
// The queue that holds messages sent to the PWM task.
//
//*****************************************************************************

xQueueHandle g_pPWMQueue;
extern xQueueHandle  g_pDisplayQueue;

extern xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// This task toggles the user selected PWM at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
static void
PWMTask(void *pvParameters)
{
    uint8_t i8Message;
    uint8_t i8DisplayMessage;

    //
    // Loop forever.
    //
    while(1)
    {
    	//*****************************************************************************
    	//
    	// MAIN ROTOR
    	//
    	//*****************************************************************************
        //
        // Read the next message, if available on queue.
        //

        if(xQueueReceive(g_pPWMQueue, &i8Message, 0) == pdPASS)
        {
            //
            // If left button, decrement Main Rotor PWM by 10%.
            //
            if(i8Message == LEFT_BUTTON)
            {

            	i8DisplayMessage = DISPLAY;

            	g_dutyMain -= 10;
            	g_dutyMain = (g_dutyMain == 88 ? 90 : g_dutyMain); 	//If duty cycle max next decrement is to 90%
            	g_dutyMain = (g_dutyMain < 2 ? 2 : g_dutyMain);		//Set min duty cycle to 2%
            	ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 1000*g_dutyMain/100);
            	ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_3);
            	ROM_PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);
            	i8DisplayMessage = DISPLAY;

            	//Update UART display
            	if(xQueueSend(g_pDisplayQueue, &i8DisplayMessage, portMAX_DELAY) !=
            	   pdPASS) {
            		//
            		// Error. The queue should never be full. If so print the
            		// error message on UART and wait for ever.
            		//
            		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
            		printf("\nQueue full. This should never happen.\n");
            		xSemaphoreGive(g_pUARTSemaphore);
            		while(1)
            		{
            		}
            	}

            }

            //
            // If right button, increment Main Rotor PWM by 10%..
            //
            if(i8Message == RIGHT_BUTTON)
            {
            	g_dutyMain += 10;
            	g_dutyMain = (g_dutyMain == 12 ? 10 : g_dutyMain); 	//If duty cycle at min next increment is to 10%
            	g_dutyMain = (g_dutyMain > 98 ? 98 : g_dutyMain);	//Set max duty cycle to 98%
            	ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 1000*g_dutyMain/100);
            	ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_3);
            	ROM_PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);
            	i8DisplayMessage = DISPLAY;

            	//Update UART display
            	if(xQueueSend(g_pDisplayQueue, &i8DisplayMessage, portMAX_DELAY) !=
            	   pdPASS) {
            		//
            		// Error. The queue should never be full. If so print the
            		// error message on UART and wait for ever.
            		//
            		xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
            		printf("\nQueue full. This should never happen.\n");
            		xSemaphoreGive(g_pUARTSemaphore);
            		while(1)
            		{
            		}

            	}
            }
        }
   }
}

//*****************************************************************************
//
// Initializes the PWM task.
//
//*****************************************************************************
uint32_t
PWMTaskInit(void)
{
    //
    // Enable the PWM0 peripheral
    //

	ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    //
    // Wait for the PWM0 module to be ready.
    //
    while(!ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
    {
    }

    //Pin Configure
    ROM_GPIOPinConfigure(GPIO_PC5_M0PWM7); //PWM Main
    ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5); //PWM Tail
    ROM_GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);
    ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);

    // Configure the PWM generator for count down mode with immediate updates
    // to the parameters.
    //
    ROM_PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    //
    //Freq = 200 Hz
    //Clock = 50MHz
    //Clock Ticks = 50e6 / 200
    //
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, 1000);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 1000);
    //
    // Set the pulse width of PWM0 for a 25% duty cycle.
    //
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 200);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 200);
    //
    // Start the timers in generator 0.
    //
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_3);
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    //
    // Enable the outputs.
    //
    ROM_PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);


    //
    // Create a queue for sending messages to the PWM task.
    //
     g_pPWMQueue = xQueueCreate(PWM_QUEUE_SIZE, PWM_ITEM_SIZE);

    //
    // Create the PWM task.
    //
    if(xTaskCreate(PWMTask, (const portCHAR *)"PWM", PWMTASKSTACKSIZE, NULL,
    	                   tskIDLE_PRIORITY + PRIORITY_PWM_TASK, NULL) != pdTRUE)
    {
    	return(1);
    }

    //
    // Success.
    //
    return(0);

}
