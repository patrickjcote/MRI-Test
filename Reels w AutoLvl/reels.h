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

#define ADXL_ADDR 0x3A		//Accelerometer Address
#define PWM_MAX	4000		// PWM high limit
#define PWM_MIN  2000		// PWM low limit
#define PWM_NEU  3000		// PWM Neutral limit
#define SAMPLES 10
#define SAMPLE_TIMER 400
#define PI 3.14159265
#define ANGLE_TOLERANCE 7
#define REELING_ANGLE 10
#define LOW_PASS_LVL 10


#define PWM_MAX2 4000		// PWM high limit
#define PWM_MIN2 2000		// PWM low limit
#define PWM_NEU 3000		// PWM Neutral limit

extern volatile int status_code, interrupt_code, ALL_STOP_FLAG;
extern volatile int cur_reel_depth, reel_dir, set_reel_depth, reel_flag, pu_flag, k;
extern volatile unsigned int timeout_count1, timeout_count2, pwmread, pwmval;

extern unsigned char TXData[],RXData[];		//Buffers for the Slave of ths device
extern volatile int TXData_ptr,RXData_ptr;		//Pointers and flags for the slave device
extern volatile unsigned int sample_count, autolvl_flg;
extern volatile int avg_angle[], set_angle, lvl_compare, avg_pointer;
extern float current_angle, average;


void init_ADXL(void);
void read_ADXL(int *);
int autoLevel();


void initReel();
int goToClick(int);
int setReelLevel(int);
int conv_char_hex(char *,int );



#endif
