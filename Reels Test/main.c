#include <msp430.h>
#include "reels.h"

volatile int clicks = 0;
volatile int motorPosition = 0;

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	initReel();

	__bis_SR_register(GIE);

	motorPosition = 20;

	pullUpReel();



	return 0;
}//main()


void initReel(){

	//Limit Switch in
	P1DIR &= ~BIT4;				// Limit switch input on 1.4
	P1IE |=  BIT4;				// 1.4 interrupt enabled
	P1IES |= BIT4;				// 1.4 Hi/lo edge
	P1REN |= BIT4;				// Enable Pull Up on SW2 (P1.4)
	P1IFG &= ~BIT4;				// P1.4 IFG clear
	//Click Count in
	P2DIR &= ~BIT0;				// Limit switch input on 2.0
	P2IE |=  BIT0;				// P2.0 interrupt enabled
	P2IES |= BIT0;				// P2.0 Hi/lo edge
	P2REN |= BIT0;				// Enable Pull Up on SW2 (P2.0)
	P2IFG &= ~BIT0;				// P2.0 IFG clear
	//Motor PWM Out
	P2DIR |= BIT4;				//PWM Output on P2.4
	P2SEL |= BIT4;				//TA1.2 Output on P2.4
	TA1CTL = TASSEL_2 + MC_1;	//Set Clock
	TA1CCTL2 |= OUTMOD_7;		//Output mode
	TA1CCR0 = PWM_PERIOD;
	TA1CCR2 = MOTOR_STOP;

	clicks = 0;

}//init_reel()

void pullUpReel(){

	while(motorPosition > 0){
		TA1CCR2 = MOTOR_UP;
	}
	TA1CCR2 = MOTOR_STOP;
}



// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	motorPosition = 0;
	P1IFG &= ~BIT4;
}


// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	clicks++;
	P2IFG &= ~BIT0;
}


