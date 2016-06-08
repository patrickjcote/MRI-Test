#include <msp430.h>

volatile int clicks = 0;
volatile int dir = 0;

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	init_reel();

	__bis_SR_register(GIE);

	return 0;
}//main()

void init_reel(){

	//Limit Switch in
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
	TA1CCR0 = 21000;
	TA1CCR2 = 1580;

}//init_reels()

int motorDir(int direction){
	TA1CCR2 = 1580 + direction*200;
}// motorDirection()

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	clicks++;
	if(clicks > 6){
		dir ^= 1;
		motorDir(dir);
		clicks = 0;
	}

	P2IFG &= ~BIT0;
}
