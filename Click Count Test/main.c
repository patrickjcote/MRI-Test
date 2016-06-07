#include <msp430.h> 

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	P2DIR = 0xff;
	P2OUT = 0x00;
	P2SEL = BIT1;

	TA1CTL = TASSEL_2 + MC_1;
	TA1CCTL1 |= OUTMOD_7;
	TA1CCR0 = 21000;
	TA1CCR1 = 1500;



	while(1)
	{
		TA1CCR1 = 1590;
		__delay_cycles(500000);
		__delay_cycles(500000);
		__delay_cycles(500000);
		TA1CCR1 = 1300;
		__delay_cycles(500000);
		__delay_cycles(500000);
		__delay_cycles(500000);
		TA1CCR1 = 1590;
		__delay_cycles(500000);
		__delay_cycles(500000);
		__delay_cycles(500000);
		TA1CCR1 = 1700;
		__delay_cycles(500000);
		__delay_cycles(500000);
		__delay_cycles(500000);



	}


	return 0;
}


