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
#define ANGLE_TOLERANCE 1	// Degrees
#define REELING_ANGLE 4		// Degrees
#define TURNS_PER_WRAP 45	// (Width of reel)/(Hose OD)
#define REEL_TIMEOUT 6		// Seconds
#define TOP_WRAP_ANGLE 1 	// 0-Up 1-Down

// -----------------------------------------------------


#define REEL_TIMEOUT_1 30000
#define MAX_CLICKS 999		// Upper bound of allowable clicks
#define MIN_CLICKS -5		// Lower bound of allowable clicks
#define LIMIT_SWITCH_MIN 5	// Max # of clicks w/o Limit Switch error code
#define ADXL_ADDR 0x3A		// Accelerometer Address
#define PWM_MAX	4000		// PWM high limit
#define PWM_MIN  2000		// PWM low limit
#define PWM_NEU  3000		// PWM Neutral limit
#define SAMPLES 10			// Auto level averaging
#define SAMPLE_TIMER 400	// Auto level averaging
#define PI 3.14159265
#define LOW_PASS 10			// Low pass filter samples

extern unsigned char TXData[],RXData[];		//Buffers for the Slave of ths device
extern volatile int TXData_ptr,RXData_ptr;		//Pointers and flags for the slave device
extern volatile int cur_reel_depth, reel_dir, set_reel_depth, k;
extern volatile unsigned int timeout_count1, timeout_count2, pwmread, pwmval;
extern volatile int avg_angle[], set_angle, lvl_compare;
extern volatile char status_code, interrupt_code, avg_pointer;
extern volatile char pwm_pullup_flag, ALL_STOP_FLAG, reel_flag, autolevel_flag, get_level_flag;
extern float current_angle, average;


void init_ADXL(void);
void read_ADXL(int *);
void initReel();
int goToClick(int);
void getLevel();
void autoLevel();
int conv_char_hex(char *,int );



#endif
