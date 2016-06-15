/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

#define PWM_MAX	4000		// PWM high limit
#define PWM_MIN  2000		// PWM low limit
#define PWM_NEU  3000		// PWM Neutral limit
#define TURNS_PER_WRAP 5


void initReel();
int goToClick(int);
int setReelLevel();
int conv_char_hex(char *,int );

#endif
