/*
 * Authors: Lakshay Duggal, Tim Oorschot
 *
 * HeightandTail.c - calculates the position of the height and yaw of the helicopter
 */

#include "Globals.h"

float step = 0.01; // Step size for calculations

struct height_s  mainRotor_f(struct height_s mainRotor, struct height_s PWM)
{
	/* First block in the helicopter model, relating to hieght, provided by Prof. Phil Bones
	 * transfer function in the form of: 	k_1
	 * 									-----------
	 * 								     tau_1s + 1
	 */

	// P Gains
	float k_1 = 1;
	float tau_1 = 0.1;
	float mainPrevious = mainRotor.current;
	float mainCurrent;

	struct height_s temp;

	mainCurrent = (k_1 * step * (PWM.current + PWM.previous) + (2 * tau_1 - step) * mainPrevious) / (2 * tau_1 + step);

	temp.current = mainCurrent;
	temp.previous = mainRotor.current;

	return temp;

}

struct height_s mainForcePWM_f (struct height_s mainRotor) //LUTOne
{
	/* Second block in the helicopter model, relating to hieght, provided by Prof. Phil Bones
	 * Uses look up table and subtracts (gravity) x (mass of the helicopter)
	 */

	// First row of the LUT
	float LUT_PWM1[17] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80};
	// Second row
	float LUT_PWM2[17] = {0, 0.0268, 0.1609, 0.3142, 0.4748, 0.6190, 0.7626, 0.8796, 1.0104,
							1.1098, 1.2155, 1.3227, 1.4339, 1.5124, 1.6039, 1.6824, 1.7707};
	int i = 0;
	float LUTCurrent = 0;
	float LUTPrevious = 0;

	float g = 9.8067;
	float M_h = 0.0853;

	struct height_s result;
	float rotorCurrent = mainRotor.current;
	float rotorPrevious = mainRotor.previous;

	// Ensuring the input value does not cause wrap around.
	if (rotorCurrent > 80) {
		rotorCurrent = 80;
	}
	if (rotorPrevious > 80) {
		rotorPrevious = 80;
	}

	for ( i = 0; i < 17; i++) {
		/* Finding the corropoining value in the first row and interpelating between the values in the
		 * second row
		 */
		if (rotorCurrent <= LUT_PWM1[i+1] && rotorCurrent >= LUT_PWM1[i]) {
			// Interpelating between values
			LUTCurrent = ((rotorCurrent - LUT_PWM1[i])/5)*(LUT_PWM2[i+1]-LUT_PWM2[i]) + LUT_PWM2[i];
		}
		if (rotorPrevious <= LUT_PWM1[i+1] && rotorPrevious >= LUT_PWM1[i]) {
			// Interpelating between values
			LUTPrevious = ((rotorPrevious - LUT_PWM1[i])/5)*(LUT_PWM2[i+1]-LUT_PWM2[i]) + LUT_PWM2[i];
		}
	}

	// Subtracting (gravity) x (mass)
	result.current = LUTCurrent - (g*M_h);
	result.previous = LUTPrevious - (g*M_h);

	return result;
}


struct height_s heliMount_f (struct height_s heliMount, struct height_s constantOne)
{
	/* Third block in the helicopter model, relating to hieght, provided by Prof. Phil Bones
	 * transfer function in the form of: 	k_2
	 * 									-----------
	 * 								     tau_2s + 1
	 */

	float k_2 = 1.5300;
	float tau_2 = 0.638;

	struct height_s temp;

	float mountPrevious = heliMount.current;
	float mountCurrent;

	// Z transforms were used for calculations
	mountCurrent = (k_2 * step * (constantOne.current + constantOne.previous) + (2 * tau_2 - step) * mountPrevious) / (2 * tau_2 + step);

	temp.current = mountCurrent;
	temp.previous = heliMount.current;

	return temp;
}

struct height_s integrator_f (struct height_s integrator, struct height_s heliMount)
{
	/* Fourth block in the helicopter model, relating to hieght, provided by Prof. Phil Bones
	 * transfer function in the form of:   1
	 * 									  ---
	 * 								       s
	 */

	struct height_s temp;

	float intCurrent;
	float intPrevious = integrator.current;

	intCurrent = 0.5 * (step * (heliMount.current + heliMount.previous) + 2 * intPrevious);

	// The integrator is limited to 1 and -1
	if (intCurrent > 1) {
		intCurrent = 1;
	}
	else if (intCurrent < -1) {
		intCurrent = -1;
	}

	temp.current = intCurrent;
	temp.previous = integrator.current;

	return temp;
}

struct height_s LUTTwo_f (struct height_s integrator)
{
	/* Second block in the helicopter model, relating to hieght, provided by Prof. Phil Bones
	 * Uses look up table
	 */

	// First row of the lock up table
	float LUT_H_b[11] = {-1.0, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0};
	// second row of the look up table
	float LUT_H_d[11] = {46.5203, 57.0280, 67.6609, 78.3881, 89.1783, 100.000, 110.8217,
						121.6119, 132.3391, 142.9720, 153.4797};

	float LUTCurrent;
	float LUTPrevious;

	struct height_s result;

	int i = 0;

	for ( i = 0; i < 11; i++) {
			if (integrator.current <= LUT_H_b[i+1] && integrator.current >= LUT_H_b[i]) {
				// Interpelating between values
				LUTCurrent = ((integrator.current - LUT_H_b[i])/0.2)*(LUT_H_d[i+1]-LUT_H_d[i]) + LUT_H_d[i];
			}
			if (integrator.previous <= LUT_H_b[i+1] && integrator.previous >= LUT_H_b[i]) {
				// Interpelating between values
				LUTPrevious = ((integrator.previous - LUT_H_b[i])/0.2)*(LUT_H_d[i+1]-LUT_H_d[i]) + LUT_H_d[i];
			}
		}
	result.current = LUTCurrent;
	result.previous = LUTPrevious;

	return result;
}

struct height_s mainMotorTorque_f (struct height_s mainMotorTorque, struct height_s PWM)
{
	/* First block in the helicopter model, relating to torque, provided by Prof. Phil Bones
	 * transfer function in the form of: 	k_3
	 * 									-----------
	 * 								     tau_3s + 1
	 */
	float k_3 = 0.0000283; //2.83e-5;
	float tau_3 = 0.1;

	float torqueCurrent;
	float torquePrevious = mainMotorTorque.current;

	struct height_s result;

	torqueCurrent = (k_3 * step * (PWM.current + PWM.previous) + (2 * tau_3 - step) * torquePrevious) / (2 * tau_3 + step);


	result.current = torqueCurrent;
	result.previous = mainMotorTorque.current;

	return result;

}

/*
 * Functions used to calculate the yaw of the helicopter
 */
struct height_s tailRotor_f (struct height_s tailRotor, struct height_s PWM)
{
	/* Frist block in the helicopter model, relating to yaw, provided by Prof. Phil Bones
	 * transfer function in the form of: 	k_11
	 * 									-----------
	 * 								     tau_11s + 1
	 */
	float k_11 = 1;
	float tau_11 = 0.1;

	float tailCurrent;
	float tailPrevious = tailRotor.current;

	struct height_s result;

	tailCurrent = (k_11 * step * (PWM.current + 0.6 * PWM.previous) + (2 * tau_11 - step) * tailPrevious) / (2 * tau_11 + step);

	result.previous = tailRotor.current;
	result.current = tailCurrent;

	return result;

}


struct height_s netTorque_f (struct height_s tailRotor, struct height_s mainMotorTorque) //LUTThree
{
	/* Second block in the helicopter model, relating to yaw, provided by Prof. Phil Bones
	 * Uses look up table and subtracts main motor torque
	 */
	float LUT_Tail1[16] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75};
	float LUT_Tail2[16] = {0, 0.0003, 0.0005, 0.0007, 0.0010, 0.0012, 0.0014, 0.0016, 0.0018,
						0.0020, 0.0022, 0.0023, 0.0025, 0.0026, 0.0027, 0.0029};

	int i = 0;
	float LUTCurrent = 0;
	float LUTPrevious = 0;

	struct height_s result;

	float tailCurrent = tailRotor.current;
	float tailPrevious = tailRotor.previous;

	// Ensures that the input does not cause wrap around
	if (tailCurrent > 75) {
		tailCurrent = 75;
	}
	if (tailPrevious > 75) {
		tailPrevious = 75;
	}

	for ( i = 0; i < 16; i++) {
		if (tailCurrent <= LUT_Tail1[i+1] && tailCurrent >= LUT_Tail1[i]) {
			// Interpelating between values
			LUTCurrent = ((tailCurrent - LUT_Tail1[i])/5)*(LUT_Tail2[i+1]-LUT_Tail2[i]) + LUT_Tail2[i];
		}
		if (tailPrevious <= LUT_Tail1[i+1] && tailPrevious >= LUT_Tail1[i]) {
			// Interpelating between values
			LUTPrevious = ((tailPrevious - LUT_Tail1[i])/5)*(LUT_Tail2[i+1]-LUT_Tail2[i]) + LUT_Tail2[i];
		}
	}

	result.current = LUTCurrent - mainMotorTorque.current;
	result.previous = LUTPrevious - mainMotorTorque.previous;

	return result;
}

struct height_s heliBody_f (struct height_s heliBody, struct height_s netTorque)
{
	/* Thrid block in the helicopter model, relating to yaw, provided by Prof. Phil Bones
	 * transfer function in the form of: 	k_12
	 * 									-----------
	 * 								     tau_12s + 1
	 */
	float k_12 = 18000;
	float tau_12 = 0.37;

	float bodyCurrent;
	float bodyPrevious = heliBody.current;

	struct height_s result;

	bodyCurrent = (k_12 * step * (netTorque.current + netTorque.previous) + (2 * tau_12 - step) * bodyPrevious) / (2 * tau_12 + step);
	result.previous = heliBody.current;
	result.current = bodyCurrent;

	return result;
}

struct height_s yaw_f (struct height_s yaw, struct height_s heliBody)
{
	/* Fourth block in the helicopter model, relating to yaw, provided by Prof. Phil Bones
	 * transfer function in the form of:   1
	 * 									  ---
	 * 								       s
	 */

	float yawCurrent;

	struct height_s result;

	yawCurrent = 0.5 * (step * (heliBody.current + heliBody.previous) + 2 * yaw.previous);
	result.previous = yaw.current;
	result.current = yawCurrent;


	result.current = result.current;
	result.previous = result.current;

	// Resets the yaw to 0 when the helicopter has completed a rotation
	if ((result.current >= 2 * pi) || (result.current <= -2 * pi)){
		result.current = 0;
		result.previous = 0;
	}

	return result;
}

struct height_s gain_f (struct height_s netTorque)
{
	float gaink = 5000;
	struct height_s result;

	result.current = gaink * netTorque.current; //don't need the struct here I think
	result.previous = gaink * netTorque.previous;

	return result;
}

struct height_s claculateHeight(struct height_s PWM)
{
	/*
	 * Calculates the height of the helicopter using the above functions.
	 * Uses a static type so as to save the previous value.
	 */

	static struct height_s mainRotor;
	static struct height_s constantOne;
	static struct height_s heliMount;
	static struct height_s integrator = {-1, -1}; // Initialise as -1 to get correct response
	static struct height_s height;

							// AS DESCRIBED IN THE MODEL:
	mainRotor = mainRotor_f(mainRotor, PWM);			// Main rotor
	constantOne = mainForcePWM_f(mainRotor);			// LUT: Main force v PWM - Constant
	heliMount = heliMount_f(heliMount, constantOne);	// Helicopter Mount
	integrator = integrator_f(integrator, heliMount);	// Integrator Limited
	height = LUTTwo_f(integrator);						// LUT: %Height from angle

	return height;
}


struct height_s calculateYaw(struct height_s PWM, struct height_s PWMTail)
{
	/*
	 * Calculates the current yaw of the helicopter.
	 * Uses a static type so as to save the previous value.
	 */

	static struct height_s mainMotorTorque;
	static struct height_s tailRotor;
	static struct height_s netTorque;
	static struct height_s heliBody;
	static struct height_s yaw;
	static struct height_s gain;

									// AS DESCRIBED IN THE MODEL:
	mainMotorTorque = mainMotorTorque_f(mainMotorTorque, PWM);	// Main rotor1

	// TAIL
	tailRotor = tailRotor_f(tailRotor, PWMTail);				// Tail rotor
	netTorque = netTorque_f(tailRotor, mainMotorTorque);		// LUT: Tail torque v PWM - Main rotor1
	heliBody = heliBody_f(heliBody, netTorque);					// Helicopter body
	yaw = yaw_f(yaw, heliBody);									// Integrator

	// GAIN
	gain = gain_f(netTorque);									// Gain

	return yaw;
}
