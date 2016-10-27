/*
 * servo_timer.c
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */
#include  "msp430.h"
#define ti_quarter 18000
unsigned int motor[8],tcounter=0,ti_run=0, motors_en=0;
//,PWM_SET[]={0x0F,0x0F,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00},PWM_CLR[]={0x00,0x00,0x00,0x00,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F},PWM_CTR[]={8500,35000,8500,8500,60800,60800,60800,60800,31800};
//void calc_PWM(void);
void pwm_init(int motors_on){
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	CCTL0 = CCIE;                           // TACCR0 interrupt enabled
	CCR0 = 50000;
	TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

	P1DIR |=motors_on;
	motors_en=motors_on;
	motor[0]=0;
	motor[1]=0;
	motor[2]=0;
	motor[3]=0;
	motor[4]=0;
	motor[5]=0;
	motor[6]=0;
	motor[7]=0;
	__bis_SR_register(GIE);       			// interrupts enabled

}




#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	volatile int temp;
	if ((ti_run==0)&&(motor[tcounter]>85)){
		CCR0+=motor[tcounter];
		if (motors_en&(1<<tcounter))
			P1OUT|=(1<<tcounter);
		ti_run=0;
	}
	else if(ti_run==0)
		ti_run++;
	if (ti_run==1){
		if (motors_en&(1<<tcounter))
			P1OUT&=~(1<<tcounter);
		if(motor[tcounter]<=85){
			CCR0+=ti_quarter;
		}
		else
			CCR0+=(ti_quarter-motor[temp]);
	}
	if (ti_run>1){
		tcounter++;
		CCR0+=ti_quarter;
	}
	ti_run++;
	if (ti_run>2)
		ti_run=0;
	if(tcounter>7)
		tcounter=0;

}

