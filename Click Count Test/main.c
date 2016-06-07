#include <msp430.h>

#define STOP 0
#define CCW -100
#define CW 100

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;

	P2DIR = BIT1;
	P2OUT = 0x00;
	P2SEL = BIT1;

	P2DIR &= ~BIT0;

	TA1CTL = TASSEL_2 + MC_1;
	TA1CCTL1 |= OUTMOD_7;
	TA1CCR0 = 21000;
	TA1CCR1 = 1500;

	volatile int clicks = 0;
	while(1){
		motorDir(CCW);
		while(clicks < 6){
			clickWait();
			clicks++;
		}//while
		motorDir(CW);
		clicks = 0;
		while(clicks < 6){
			clickWait();
			clicks++;
		}//while
		clicks = 0;
	}

	return 0;
}

void clickWait(){
	while(P2IN & BIT0){
		//no click
	}
	while(!(P2IN & BIT0)){
		//click
	}
}

int motorDir(int direction){
	TA1CCR1 = 1590 + direction;
}// motorDirection()
