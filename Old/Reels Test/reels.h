/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

#define PWM_PERIOD 40000
#define MOTOR_STOP 3000
#define MOTOR_UP 2000
#define MOTOR_DOWN 4000


extern volatile int clicks;
extern volatile int reelDepth;

void initReel();
int currentDepth();
int goToDepth(int feet);
int pullUpReel();

#endif
