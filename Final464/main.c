#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils/uartstdio.h"
#include "Globals.h"
#include "HeightandTail.h"
#include "setup.h"
#include "interrupts.h"
#include "sendData.h"
//#include ""

int main() {

	//ConfigureUART();
	setupInit();

	UARTprintf("test hi\n");

	//Main Rotor

	PWM.current = 0;
	PWM.previous = 0;

	PWMTail.current = 0;
	PWMTail.previous = 0;

	struct height_s height;
	height.current = 0;
	height.previous = 0;

	struct height_s yaw;
	yaw.current = 0;
	yaw.previous = 0;

	struct height_s yawDeg;
	yawDeg.current = 0;
	yawDeg.previous = 0;

	int i = 0;

	int duck;
	int cat;
	int goose;
	int mouse;

	while(1) {

		if (i>12500) {


		//PWMTail.current = getTailPWM();
		height = claculateHeight(PWM);
		sendHeight(height);

		yaw = calculateYaw(PWM, PWMTail);

		yawDeg = referncePoint(yaw);
		generateQuad(yawDeg);

		duck = height.current;
		goose = PWM.current;
		cat  = yaw.current*180/pi;
		mouse = PWMTail.current;


		UARTprintf("height = %d (%d), yaw = %d (%d) \n", duck, goose, cat, mouse);
		i=0;
		}

		else {
			i++;
		}
	}
}

