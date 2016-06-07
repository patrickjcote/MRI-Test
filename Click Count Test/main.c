#include <msp430.h>

volatile int clicks = 0;
volatile int dir = 0;

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	P2DIR = BIT1;
	P2OUT = 0x00;
	P2SEL = BIT1;

	P2DIR &= ~BIT0;
	P2IE |=  BIT0;                            // P2.0 interrupt enabled
	P2IES |= BIT0;                            // P2.0 Hi/lo edge
	P2REN |= BIT0;                            // Enable Pull Up on SW2 (P2.0)
	P2IFG &= ~BIT0;                           // P2.0 IFG clear

	TA1CTL = TASSEL_2 + MC_1;
	TA1CCTL1 |= OUTMOD_7;
	TA1CCR0 = 21000;
	TA1CCR1 = 1500;

	__bis_SR_register(GIE);

	return 0;
}//main()

int motorDir(int direction){
	TA1CCR1 = 1500 + direction*200;
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
