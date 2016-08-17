#include <msp430.h> 
#include "stepper.h"

void initStepper(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	BCSCTL1 = CALBC1_16MHZ;		// Set Clock Speed
	DCOCTL = CALDCO_16MHZ;

	// Direction + Enable
	P1DIR |= BIT3 + BIT4;
	P1OUT |= BIT3 + BIT4;

	// Stepper PWM
	P1DIR |= BIT2;
	P1SEL |= BIT2;
	P1SEL2 &= ~BIT2;
	TA0CCR0 = 16000;			// 1kHz
	TA0CCR1 = 1600;				// 10% duty
	TA0CCTL1 = OUTMOD_7;
	TA0CCTL0 = CCIE;			// Interrupt Enable
	TA0CTL = TASSEL_2 + MC_1;	// smclock and count up

	// Stepper Limit Switch
	P1DIR &= ~BIT5;				// Limit switch input (P1.5)
	P1REN |= BIT5;				// Enable Resistor (P1.5)
	P1OUT |= BIT5;				// Pull Up Resistor (P1.5)

	stepper_pos = -1;
	stepper_dir = 0;
	stepper_en = 0;

	__delay_cycles(50000);
	__bis_SR_register(GIE);

	findHome();					// Zero position of stepper arm
}

void step(int direction){
	setDirection(direction);
	step_count = 0;
	while (step_count < STEPS_PER_CLICK ){
		stepper_en = 1;
	}
}//step()

void goToStep(int setStep){
	if(stepper_pos > setStep){
		setDirection(BACKWARD);
		while(stepper_pos > setStep){
			stepper_en = 1;
		}
	}
	else if(stepper_pos < setStep){
		setDirection(FORWARD);
		while(stepper_pos < setStep){
			stepper_en = 1;
		}
	}
}//goToStep()

void setDirection(int direction){

	volatile int n;
	stepper_dir = direction;

	if(direction == FORWARD)
		P1OUT &= ~BIT3;		// Set Direction
	if(direction == BACKWARD)
		P1OUT |= BIT3;		// Set Direction

	for(n=0;n<100;n++);		// ~5us delay


}//setDirection()

int findHome(void)
{
	volatile int n;
	setDirection(BACKWARD);

	stepper_en = 1;

	for(n=0;n<50;n++){
		while(P1IN & BIT5)
		{
			stepper_dir = BACKWARD;
		}
	}

	stepper_pos = 0;
	setDirection(FORWARD);
	stepper_en = 0;
	return 1;

}//findHome()


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	// Enable check
	if(stepper_en)
		P1OUT |= BIT4;		// Enable Motor
	else
		P1OUT &= ~BIT4;		// Disable Motor

	if(stepper_en){
		step_count++;
		stepper_pos += stepper_dir;	// Inc/Dec stepper position counter
	}
}
