/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

#define PWM_PERIOD 21000
#define MOTOR_NEUTRAL 1550
#define MOTOR_UP 1700
#define MOTOR_DOWN 1400


extern volatile int clicks = 0;
extern volatile int motorPosition;

void initReel();
void pullUpReel();

#endif
