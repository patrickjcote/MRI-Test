/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

#define PWM_PERIOD 21000
#define MOTOR_STOP 1570
#define MOTOR_UP 1700
#define MOTOR_DOWN 1400


extern volatile int clicks;
extern volatile int reelDepth;

void initReel();
int currentDepth();
int goToDepth(int feet);
int pullUpReel();

#endif
