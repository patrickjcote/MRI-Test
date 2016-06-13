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
#define REEL_TIMEOUT_1 50000
#define REEL_TIMEOUT_2 5


extern volatile int cur_reel_depth, reel_dir, set_reel_depth, set_reel_level, ALL_STOP_FLAG, reel_flag;
extern volatile unsigned int timeout_count1, timeout_count2;

void initReel();
int goToClick(int);
int setReelLevel();
int conv_char_hex(char *,int );

#endif
