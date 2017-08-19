//*****************************************************************************
//
// PWM_task.c - A simple flashing PWM task.
//
// Copyright (c) 2012-2016 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.3.156 of the EK-TM4C123GXL Firmware Package.
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
#define PWMTASKSTACKSIZE        128         // Stack size in words

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

int8_t dutyMain, dutyRotor;


//*****************************************************************************
//
// The queue that holds messages sent to the PWM task.
//
//*****************************************************************************
xQueueHandle g_pPWMQueue;

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
    portTickType ui32WakeTime;
    uint32_t ui32PWMToggleDelay;
    uint8_t i8Message;

    //
    // Get the current tick count.
    //
    ui32WakeTime = xTaskGetTickCount();

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
            // If left button, update to next PWM.
            //
            if(i8Message == LEFT_BUTTON)
            {
            	dutyMain -= 10;
            	dutyMain = (dutyMain < 2 ? 2 : dutyMain);
            	ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 250*dutyMain/100);
            	ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_3);
            	ROM_PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);

            }

            //
            // If right button, update delay time between toggles of PWM.
            //
            if(i8Message == RIGHT_BUTTON)
            {
            	dutyMain += 10;
            	dutyMain = (dutyMain > 98 ? 98 : dutyMain);
            	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 250*dutyMain/100);
            	PWMGenEnable(PWM0_BASE, PWM_GEN_3);
            	PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);

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

	ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

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
    //Freq = 200 KHz
    //Clock = 50MHz
    //Clock Ticks = 50e6 / 200e3
    //
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, 250);
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 250);
    //
    // Set the pulse width of PWM0 for a 25% duty cycle.
    //
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 100);
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 100);
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
