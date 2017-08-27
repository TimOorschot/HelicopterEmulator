/*
 * Authors: Lakshay Duggal, Tim Oorschot
 */

#ifndef SENDDATA_H_
#define SENDDATA_H_

void generateQuad(struct height_s yaw);
// Generates a quadrature signal based on the direction of the helicopter.

struct height_s referncePoint(struct height_s);
// Generates a pulse on the reference pin every time the heli passes the 0degree value

void sendHeight(struct height_s );
// Sends the height of the helicopter to the DAC using SPI

#endif /* SENDDATA_H_ */
