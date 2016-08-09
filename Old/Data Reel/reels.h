/*
 * reels.h
 *
 *  Created on: June 8, 2016
 *      Author: PCote
 */

#ifndef REELS_H_
#define REELS_H_

// ------------ Tuning Parameters ----------------------

#define MOTOR_UP 3500		// PWM high limit
#define MOTOR_DOWN 2500		// PWM low limit
#define TURNS_PER_WRAP 28	// (Width of reel)/(Wire OD)
#define REEL_TIMEOUT 8		// Seconds

// -----------------------------------------------------


#define REEL_TIMEOUT_1 50000
#define MAX_CLICKS 999		//Upper bound of allowable clicks
#define MIN_CLICKS -5		//Lower bound of allowable clicks
#define LIMIT_SWITCH_MIN 5	//Max # of clicks w/o Limit Switch error codes
#define PWM_NEU  3000		// PWM Neutral limit


extern unsigned char TXData[],RXData[];		//Buffers for the Slave of ths device
extern volatile int TXData_ptr,RXData_ptr;		//Pointers and flags for the slave device
extern volatile int cur_reel_depth, reel_dir, set_reel_depth, k;
extern volatile unsigned int timeout_count1, timeout_count2, pwmread, pwmval;
extern volatile char status_code, interrupt_code;
extern volatile char pwm_pullup_flag, ALL_STOP_FLAG, reel_flag;


void initReel();
int goToClick(int);
int conv_char_hex(char *,int );



#endif
