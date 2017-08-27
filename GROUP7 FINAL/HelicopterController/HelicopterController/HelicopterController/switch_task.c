//*****************************************************************************
//
// switch_task.c -  Initialises and describes button behaviour for Helicopter Controller
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
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/buttons.h"
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
#define SWITCHTASKSTACKSIZE        256         // Stack size in words

extern xQueueHandle  g_pPWMQueue;
extern xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// This task reads the buttons' state and passes this information to PWMTask.
//
//*****************************************************************************
static void
SwitchTask(void *pvParameters)
{
	// Set Task wait time (button polling time)
	uint32_t ui32SwitchDelay = 20;

    portTickType ui16LastTime;
    uint8_t ui8CurButtonState, ui8PrevButtonState;
    uint8_t ui8Message;

    ui8CurButtonState = ui8PrevButtonState = 0;

    //
    // Get the current tick count.
    //
    ui16LastTime = xTaskGetTickCount();

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Poll the debounced state of the buttons.
        //
        ui8CurButtonState = ButtonsPoll(0, 0);

        //
        // Check if previous debounced state is equal to the current state.
        //
        if(ui8CurButtonState != ui8PrevButtonState)
        {
            ui8PrevButtonState = ui8CurButtonState;

            //
            // Check to make sure the change in state is due to button press
            // and not due to button release.
            //
            if((ui8CurButtonState & ALL_BUTTONS) != 0)
            {
                if((ui8CurButtonState & ALL_BUTTONS) == LEFT_BUTTON)
                {
                    ui8Message = LEFT_BUTTON;
                }
                else if((ui8CurButtonState & ALL_BUTTONS) == RIGHT_BUTTON)
                {
                    ui8Message = RIGHT_BUTTON;
                }

                //
                // Pass the value of the button pressed to PWMTask.
                //
                if(xQueueSend(g_pPWMQueue, &ui8Message, portMAX_DELAY) !=
                   pdPASS)
                {
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

        //
        // Wait for the required amount of time to check back.
        //
        vTaskDelayUntil(&ui16LastTime, ui32SwitchDelay / portTICK_RATE_MS);
    }
}

//*****************************************************************************
//
// Initializes the switch task.
//
//*****************************************************************************
uint32_t
SwitchTaskInit(void)
{
    //
    // Unlock the GPIO LOCK register for Right button to work.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0xFF;

    //
    // Initialize the buttons
    //
    ButtonsInit();

    //
    // Create the switch task.
    //
    if(xTaskCreate(SwitchTask, (const portCHAR *)"Switch",
                   SWITCHTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_SWITCH_TASK, NULL) != pdTRUE)
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
