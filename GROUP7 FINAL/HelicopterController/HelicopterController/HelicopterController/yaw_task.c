//*****************************************************************************
//
// Yaw_task.c - A task used to decode the quadrature signal to determine the
// yaw of the helicopter.
// This uses interrupts.
//
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
// Global Variables
//
//*****************************************************************************

extern uint32_t g_Altitude[1];
extern uint8_t g_Direction;
extern uint32_t g_Yaw;
xSemaphoreHandle binary_sem;

//*****************************************************************************
//
// Local Variables
//
//*****************************************************************************

uint8_t seqA, seqB;
xQueueHandle queue_handle = NULL;
long prevState = 0;
long notches = 0;

//*****************************************************************************
//
// Interrupt Handler
//
//*****************************************************************************

static void
PinChangeInt(void){

	// Triggers on both edges of PB0 and PB1

	//Reads pins PB0 and PB1
	long currentState = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//Sends the read values to the queue (Use FromISR to keep ISR short)
	xQueueSendFromISR(queue_handle, &currentState, portMAX_DELAY);

	//Clears interrupt on PB0 and PB1
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

}

static void
YawTask(void *pvParameters)
{
	//current queue item
	long currentState;

	//LUT for decoding
	long yawList[16] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};


	while(1){

		//Check if item is in queue and is willing to wait forever
		if(xQueueReceive(queue_handle, &currentState, portMAX_DELAY)){

			//Computes index for the LUT
			long index = currentState | prevState;

			//Number of notches in quadrature (448 in 360 degrees)
			notches += yawList[index];

			//Check if g_Yaw is being used
			if(xSemaphoreTake(binary_sem, portMAX_DELAY)){
				//Convert notches to degrees and set it
				g_Yaw = (notches * 360 / 448) % 360;
				//Finished with g_Yaw (Allow other tasks to use it)
				xSemaphoreGive(binary_sem);
			}

			//Debug
			UARTprintf("\n\n Deg: %d \n\n", g_Yaw);

			//Set previous state to current state bit shifted left by 2
			//Used for index LUT
			prevState = (currentState << 2);
		}

	}

}

uint32_t
YawTaskInit(void)
{
	//Init interrupt on PB0 and PB1 and set handler
	GPIOIntRegister(GPIO_PORTB_BASE, PinChangeInt);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 );
	ROM_GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);
	ROM_IntMasterEnable();
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//Create semaphore for g_Yaw
	vSemaphoreCreateBinary(binary_sem);

	//Create queue used by yawTask
	queue_handle = xQueueCreate(100, sizeof(long));

	//Create yawTask
	if(xTaskCreate(YawTask, (const portCHAR *)"Yaw",
				   YAWTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
				   PRIORITY_YAW_TASK, NULL) != pdTRUE)
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



