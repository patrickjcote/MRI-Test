/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

#define ADXL_ADDR 0x3A		// ADXL345 Accelerometer Address
#define PWM_MAX	4000		// PWM high limit
#define PWM_MIN  2000		// PWM low limit
#define PWM_NEU  3000		// PWM Neutral limit

#define TURNS_PER_WRAP 5


extern volatile int currentClick;

void initReel();
int currentDepth();
int goToDepth(int feet);
int conv_char_hex(char *,int );
void init_ADXL(void);
void read_ADXL(int *, int *);
int setLevel();

#endif
