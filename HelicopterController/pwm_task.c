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
   // portTickType ui32WakeTime;
   // uint32_t ui32PWMToggPWMelay;
    uint8_t i8Message;

    //
    // Get the current tick count.
    //
    //ui32WakeTime = xTaskGetTickCount();

    //
    // Loop forever.
    //
    while(1)
    {
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
            	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 200);
            	PWMGenEnable(PWM0_BASE, PWM_GEN_3);
            	PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);

                /*
                // Guard UART from concurrent access. Print the currently
                // blinking PWM.
                //
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("PWM %d is blinking. [R, G, B]\n", g_ui8ColorsIndx);
                xSemaphoreGive(g_pUARTSemaphore);
//              */
            }

            //
            // If right button, update delay time between toggles of PWM.
            //
            if(i8Message == RIGHT_BUTTON)
            {
            	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 100);
            	PWMGenEnable(PWM0_BASE, PWM_GEN_3);
            	PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);

                /*
                // Guard UART from concurrent access. Print the currently
                // blinking PWM.
                //
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("PWM %d is blinking. [R, G, B]\n", g_ui8ColorsIndx);
                xSemaphoreGive(g_pUARTSemaphore);
                 */
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
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 200);
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
