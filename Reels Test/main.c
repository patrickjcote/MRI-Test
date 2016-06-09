#include <msp430.h>
#include "reels.h"

volatile int clicks;
volatile int reelDepth;

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	initReel();

	reelDepth = 20;

	pullUpReel();

	return 0;
}//main()


void initReel(){

	//Limit Switch input
	P1DIR &= ~BIT4;				// Limit switch input on 1.4
	P1IE |=  BIT4;				// 1.4 interrupt enabled
	P1IES |= BIT4;				// 1.4 Hi/lo edge
	P1REN |= BIT4;				// Enable Pull Up (P1.4)
	P1IFG &= ~BIT4;				// P1.4 IFG clear
	//Click Count input
	P2DIR &= ~BIT0;				// Click count input on 2.0
	P2IE |=  BIT0;				// P2.0 interrupt enabled
	P2IES |= BIT0;				// P2.0 Hi/lo edge
	P2REN |= BIT0;				// Enable Pull Up (P2.0)
	P2IFG &= ~BIT0;				// P2.0 IFG clear
	//Motor PWM Output
	P2DIR |= BIT4;				//PWM Output on P2.4
	P2SEL |= BIT4;				//TA1.2 Output on P2.4
	TA1CTL = TASSEL_2 + MC_1;	//Set Clock
	TA1CCTL2 |= OUTMOD_7;		//Output mode
	TA1CCR0 = PWM_PERIOD;
	TA1CCR2 = MOTOR_STOP;

	clicks = 0;
	reelDepth = 0;

	__bis_SR_register(GIE);

}//init_reel()


int pullUpReel(){

	while(reelDepth > 0){
		TA1CCR2 = MOTOR_UP;
		//TODO: Autolevel code
	}
	TA1CCR2 = MOTOR_STOP;

	return 0;
}//pullUpReel()


// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	//Hardware interrupt for limit switch
	reelDepth = 0;
	P1IFG &= ~BIT4;
}

// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	//Hardware interrupt for click counter
	clicks++;
	P2IFG &= ~BIT0;
}


