/*
 * Authors: Lakshay Duggal, Tim Oorschot
 *
 * HeightandTail.H - calculates the position of the height and yaw of the helicopter
 *
 */

#ifndef HEIGHTANDTAIL_H_
#define HEIGHTANDTAIL_H_

struct height_s calculateYaw(struct height_s, struct height_s);
// Calculates the yaw of the helicopter

struct height_s claculateHeight(struct height_s);
// Calculates the height of the helicopter

#endif /* HEIGHTANDTAIL_H_ */
