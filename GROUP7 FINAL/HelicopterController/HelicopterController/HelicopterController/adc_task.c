//*****************************************************************************
//
// ADC_task.c - A simple ADC task to process the buttons.
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
#include "utils/uartstdio.h"

#include "yaw_task.h"
#include "display_task.h"
#include "adc_task.h"
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
#define ADCTASKSTACKSIZE        256       // Stack size in words

extern xQueueHandle  g_pPWMQueue;
extern xQueueHandle  g_pDisplayQueue;
extern xSemaphoreHandle g_pUARTSemaphore;


//*****************************************************************************
//
// Global g_Altitude
//
//****************************************************************************

extern uint32_t g_Altitude[1];


//*****************************************************************************
//
// This task reads the buttons' state and passes this information to ADCTask.
//
//*****************************************************************************
static void
ADCTask(void *pvParameters)
{
	portTickType ui16LastTime;
    uint32_t ui32ADCDelay = 10;
    //
    // Get the current tick count.
    //
    ui16LastTime = xTaskGetTickCount();

    //
    // Loop forever.
    //
    while(1)
    {
    	//Trigger ADC
    	ROM_ADCProcessorTrigger(ADC0_BASE, 3);

    	// Wait for conversion to be completed.
    	while(!ROM_ADCIntStatus(ADC0_BASE, 3, false))
    	{
    	}

    	// Clear the ADC interrupt flag.
    	ROM_ADCIntClear(ADC0_BASE, 3);

    	// Read ADC Value.
    	ROM_ADCSequenceDataGet(ADC0_BASE, 3, g_Altitude);

    	// Print g_Altitude
    	//xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
    	//UARTprintf("g_Altitude: %4d\r \n", g_Altitude[0]);
    	//xSemaphoreGive(g_pUARTSemaphore);

        //
        // Wait for the required amount of time to check back.
        //
        vTaskDelayUntil(&ui16LastTime, ui32ADCDelay / portTICK_RATE_MS);
    }
}

//*****************************************************************************
//
// Initializes the ADC task.
//
//*****************************************************************************
uint32_t
ADCTaskInit(void)
{
	//ENABLE ADC0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//	while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)));

	//ENABLE ADC PIN
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	ROM_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);

	//ADC CONFIG
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
		                              ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC0_BASE, 3);
	ROM_ADCIntClear(ADC0_BASE, 3);

	//READ INITIAL ALTITUDE
	//Trigger ADC
	ROM_ADCProcessorTrigger(ADC0_BASE, 3);

	// Wait for conversion to be completed.
	while(!ROM_ADCIntStatus(ADC0_BASE, 3, false))
	{
	}
	// Clear the ADC interrupt flag.
	ROM_ADCIntClear(ADC0_BASE, 3);
	// Read ADC Value.
	ROM_ADCSequenceDataGet(ADC0_BASE, 3, g_Altitude);

    //
    // Create the ADC task.
    //
    if(xTaskCreate(ADCTask, (const portCHAR *)"ADC",
                   ADCTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_ADC_TASK, NULL) != pdTRUE)
    {

    	//
		//Failure (Stop from running vTaskStartScheduler())
		//
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
