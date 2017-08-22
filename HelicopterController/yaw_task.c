//*****************************************************************************
//
// Yaw_task.c - A simple Yaw task to process the buttons.
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
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"

#include "driverlib/interrupt.h"
#include "yaw_task.h"
#include "switch_task.h"
#include "pwm_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//*****************************************************************************
//
// The stack size for the yaw task.
//
//*****************************************************************************
#define YAWTASKSTACKSIZE       256      // Stack size in words

extern xQueueHandle  g_pPWMQueue;
extern xQueueHandle  g_pDisplayQueue;
extern xSemaphoreHandle g_pUARTSemaphore;


//*****************************************************************************
//
// Global Direction
//
//****************************************************************************

extern uint32_t g_Altitude[1];
extern uint8_t g_Direction;
extern uint8_t g_Yaw;
uint8_t seqA, seqB;




//*****************************************************************************
//
// This task reads the buttons' state and passes this information to YawTask.
//
/*****************************************************************************


static void
YawTask(void *pvParameters)
{
	portTickType ui16LastTime;
    uint32_t ui32YawDelay = 5;
    uint8_t YawA, YawB;
    //
    // Get the current tick count.
    //
    ui16LastTime = xTaskGetTickCount();

    //
    // Loop forever.
    //
    while(1)
    {
    	YawA = ROM_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0);
    	YawB = ROM_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);

    	// Read A and B signals

    	// Record the A and B signals in seperate sequences
    	seqA <<= 1;
    	seqA |= YawA;

    	seqB <<= 1;
    	seqB |= YawB;

    	 // Mask the MSB four bits
    	seqA &= 0b00001111;
    	seqB &= 0b00001111;

    	// Compare the recorded sequence with the expected sequence
    	if (seqA == 0b00001001 && seqB == 0b00000011) {
    	      g_Yaw--;
    	      g_Direction = 0;
    	      }

    	    if (seqA == 0b00000011 && seqB == 0b00001001) {
    	      g_Yaw++;
    	      g_Direction = 1;
    	      }

        vTaskDelayUntil(&ui16LastTime, ui32YawDelay / portTICK_RATE_MS);
    }
}

//*****************************************************************************
//
// Initializes the Yaw task.
//
//*****************************************************************************
uint32_t
YawTaskInit(void)
{

	//Initialise Yaw A (PB0) and Yaw B (PB1)Pins
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
    //
    // Create the Yaw task.
    //
    if(xTaskCreate(YawTask, (const portCHAR *)"Yaw",
                   YAWTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_YAW_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
*/

xSemaphoreHandle binary_sem;

static void
one_sec_isr(void){
	xSemaphoreGiveFromISR(binary_sem, NULL);
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);
}

static void
YawTask(void *pvParameters)
{

	while(1){
		if(xSemaphoreTake(binary_sem, portMAX_DELAY)){




//			//if (g_Direction != ROM_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1))
//					g_Yaw++;
//			//g_Direction = ROM_GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1);
//			xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//			UARTprintf("\n\n INTERRUPT %d \n\n", g_Yaw);
//			xSemaphoreGiveFromISR(g_pUARTSemaphore,NULL);
//			GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);

		}
	}
}

uint32_t
YawTaskInit(void)
{
	GPIOIntRegister(GPIO_PORTB_BASE, one_sec_isr);

	ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
	ROM_GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);

	ROM_IntMasterEnable();

	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0);
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);


	vSemaphoreCreateBinary(binary_sem);

	if(xTaskCreate(YawTask, (const portCHAR *)"Yaw",
				   YAWTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
				   PRIORITY_YAW_TASK, NULL) != pdTRUE)
	{
		return(1);
	}

    //
    // Success.
    //
    return(0);

}



