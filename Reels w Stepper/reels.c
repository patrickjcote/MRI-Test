#include <msp430.h>
#include "reels.h"



void initReel(){

	//LED Indicator
	P2DIR |= BIT5;
	P2OUT |= BIT5;

	// Bump Stop Limit Switch input
	P1DIR &= ~BUMP_STOP;				// Limit switch input
	P1IE |=  BUMP_STOP;					// Interrupt enabled
	P1IES |= BUMP_STOP;					// Hi/lo edge
	P1REN |= BUMP_STOP;					// Enable Pull Up
	P1IFG &= ~BUMP_STOP;				// IFG clear
	// Click Count input
	P2DIR &= ~CLICK_COUNTER;			// Click count input
	P2IE |=  CLICK_COUNTER;				// Interrupt enabled
	P2IES |= CLICK_COUNTER;				// Hi/lo edge
	P2REN |= CLICK_COUNTER;				// Enable Pull Up
	P2IFG &= ~CLICK_COUNTER;			// IFG clear
	// Reel Motor PWM Timer Init
	TA1CCR0 = 40000;					// PWM Period 20ms
	TA1CCR1 = 0;
	TA1CCTL1 = OUTMOD_7;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	// PWM Output
	P2DIR |= MOTOR_SIGNAL;				// Motor Control
	P2SEL |= MOTOR_SIGNAL;
	// Init Globals
	reel.currentClick = 0;
	reel.setClick = 0;
	reel.direction = STOPPED;
	reel.flag = FALSE;
	reel.currentWrap = 0;
	statusCode = 0;
	interruptCode = 0;
	allStopFlag = 1;

}//init_reel()

int goToClick(){

	//clicks timeout check
	reel.timeout1++;
	if(reel.timeout1 > REEL_TIMEOUT_1){
		reel.timeout1 = 0;
		reel.timeout2++;
	}
	if(reel.timeout2 > REEL_TIMEOUT){
		allStopFlag = 1;
		return 3;		//Clicks missed, return timeout status
	}

	//determine reel direction, check if limit switch is engaged
	if((reel.currentClick != reel.setClick) && (P1IN & BUMP_STOP)){
		if(reel.currentClick > reel.setClick){
			reel.direction = UP;
			reel.PWM = MOTOR_UP;
			stepper.flag = 1;
			return reel.direction;
		}
		if(reel.currentClick < reel.setClick){
			reel.direction = DOWN;
			reel.PWM = MOTOR_DOWN;
			stepper.flag = 1;
			return reel.direction;
		}
	}
	else{
		reel.direction = STOPPED;
		reel.flag = 0;
		reel.PWM = PWM_NEU;
	}
	if(P1IN & BUMP_STOP)	// If bump stop switch not hit
		return 0;
	else
		interruptCode = 2;	// TODO: Comment Interrupt and Status Codes
	return 5;

}//goToClick()




// --------------------------------  Interrupts ----------------------
// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IFG & BUMP_STOP){
		// Hardware interrupt for limit switch
		// Check if reeling up and if current depth is close enough to 0
		if(reel.direction == UP && reel.flag && reel.currentClick < LIMIT_SWITCH_MIN){
			reel.currentClick = 0;
			reel.direction = 0;
			reel.flag = 0;
			statusCode = 0;
			interruptCode = 1;  //Limit switch hit
		}
		else{
			reel.flag = 0;
			interruptCode = 2; // Limit switch error
		}

		allStopFlag = 1;
		P1IFG &= ~BUMP_STOP;
	}//if limit switch

}

// Port 2 ISR
// Hardware interrupt for click counter
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

	if(reel.direction == DOWN && reel.flag){
		reel.currentClick++;
		stepper.flag = 1;
	}
	if(reel.direction == UP && reel.flag){
		reel.currentClick--;
		stepper.flag = 1;
	}
	if(reel.currentClick > MAX_CLICKS){
		allStopFlag = 1;
		reel.flag = 0;
		stepper.flag = 0;
		interruptCode = 3; //Clicks out of bounds
	}
	if(reel.currentClick < MIN_CLICKS)
		reel.currentClick = 0;

	// Click hit, reset timeout counter
	reel.timeout1 = 0;
	reel.timeout2 = 0;
	// Calculate current wrap level on the reel
	reel.currentWrap = (reel.currentClick/CLICKS_PER_WRAP) + 1;

	P2IFG &= ~CLICK_COUNTER;			// Clear interrupt

}

// Timer A1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A10 (void)
{
	// Set Reel Motor PWM duty cycle
	TA1CCR2 = reel.PWM;
}
