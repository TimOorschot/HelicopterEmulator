/*
 * sendData.h
 *
 *  Created on: Aug 24, 2017
 *      Author: ldu30
 */

#ifndef SENDDATA_H_
#define SENDDATA_H_

void generateQuad(struct height_s yaw);

struct height_s referncePoint(struct height_s);

void sendHeight(struct height_s );

#endif /* SENDDATA_H_ */
