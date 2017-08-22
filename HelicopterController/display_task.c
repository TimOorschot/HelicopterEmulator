//*****************************************************************************
//
// Display_task.c - A simple Display task to process the buttons.
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
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "yaw_task.h"
#include "display_task.h"
#include "switch_task.h"
#include "pwm_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define DISPLAYTASKSTACKSIZE        256       // Stack size in words
#define	DISPLAY 0x00000001

//*****************************************************************************
//
// The item size and queue size for the PWM message queue.
//
//*****************************************************************************
#define DISPLAY_ITEM_SIZE           sizeof(uint8_t)
#define DISPLAY_QUEUE_SIZE          5

//*****************************************************************************
//
// The queue that holds messages sent to the PWM task.
//
//*****************************************************************************

xQueueHandle g_pDisplayQueue;


//*****************************************************************************
//
// Mutexes
//
//*****************************************************************************

xSemaphoreHandle g_pUARTSemaphore;


//*****************************************************************************
//
// Global Altitude
//
//****************************************************************************

extern uint32_t g_Direction;
extern uint32_t g_Altitude[1];
extern int8_t g_dutyMain, g_dutyRotor, g_Yaw;

//*****************************************************************************
//
// This task reads the buttons' state and passes this information to DisplayTask.
//
//*****************************************************************************
static void
DisplayTask(void *pvParameters)
{
    uint8_t i8DisplayMessage, dispAltitude;
    char* string;
    //
    // Get the current tick count.
    //
   // ui16LastTime = xTaskGetTickCount();

    //
    // Loop forever.
    //
    while(1)
    {

    	 if(xQueueReceive(g_pDisplayQueue, &i8DisplayMessage, 0) == pdPASS)
    	 {
    	    //
    	    //
    	    //
    		dispAltitude = 70 * (2150 - g_Altitude[0]) / 900;
    		string = ((g_Direction == 1) ? "CCW" : "CW");

    	    if(i8DisplayMessage == DISPLAY)
    	    {

    	    	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
    	    	UARTprintf("\nCurrent Altitude: %d mm \n", dispAltitude);
    	    	UARTprintf("Main Duty Cycle: %d %% \n", g_dutyMain);
    	    	UARTprintf("Rotor Duty Cycle: %d %% \n", g_dutyRotor);
    	    	UARTprintf("Yaw: %d deg\n", g_Yaw);
    	    	UARTprintf("Direcion: %s  \n", string);
    	    	xSemaphoreGive(g_pUARTSemaphore);
    	    }
    	 }
    }
}

//*****************************************************************************
//
// Initializes the Display task.
//
//*****************************************************************************
uint32_t
DisplayTaskInit(void)
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

	 //
	 // Create a queue for sending messages to the PWM task.
	 //
	 g_pDisplayQueue = xQueueCreate(DISPLAY_QUEUE_SIZE, DISPLAY_ITEM_SIZE);

	//
	// Create a mutex to guard the UART.
	//
	g_pUARTSemaphore = xSemaphoreCreateMutex();

	xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
	UARTprintf("\nYou're good!  \n");
	xSemaphoreGive(g_pUARTSemaphore);
    //
    // Create the Display task.
    //
    if(xTaskCreate(DisplayTask, (const portCHAR *)"Display",
                   DISPLAYTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_DISPLAY_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
