/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

#define TURNS_PER_WRAP 45
#define REEL_TIMEOUT_1 50000
#define REEL_TIMEOUT_2 10
#define MAX_CLICKS 999	//Upper bound of allowable clicks
#define MIN_CLICKS -5	//Lower bound of allowable clicks
#define LIMIT_SWITCH_MIN 5		//Max # of clicks w/o Limit Switch error code

#define PWM_MAX	3400		// PWM high limit
#define PWM_MIN 2600		// PWM low limit
#define PWM_NEU 3000		// PWM Neutral limit

#define PWM_MAX2 4000		// PWM high limit
#define PWM_MIN2 2000		// PWM low limit
#define PWM_NEU 3000		// PWM Neutral limit

extern volatile int status_code, interrupt_code, ALL_STOP_FLAG;
extern volatile int cur_reel_depth, reel_dir, set_reel_depth, reel_flag, pu_flag;
extern volatile unsigned int timeout_count1, timeout_count2, pwmread, pwmval;

void initReel();
int goToClick(int);
int setReelLevel(int);
int conv_char_hex(char *,int );



#endif
