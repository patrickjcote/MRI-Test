#include <msp430.h> 
#include "reels.h"

void initStepper(void) {

	// Direction Pin
	P1DIR |= STEPPER_DIR;
	P1OUT |= STEPPER_DIR;

	// Stepper PWM and Enable
	P2DIR |= STEPPER_STEP + STEPPER_EN;
	P2SEL |= STEPPER_STEP;		// TA0.1 Output on 2.6

	TA0CCR0 = 16000;			// 1kHz
	TA0CCR1 = 1600;				// 10% duty
	TA0CCTL1 = OUTMOD_7;
	TA0CCTL0 = CCIE;			// Interrupt Enable
	TA0CTL = TASSEL_2 + MC_1;	// smclock and count up

	// Stepper Limit Switch
	P1DIR &= ~STEPPER_LIMIT;			// Limit switch input
	P1REN |= STEPPER_LIMIT;				// Enable Resistor
	P1OUT |= STEPPER_LIMIT;				// Pull Up Resistor

	// Init Stepper Struct
	stepper.position = -1;
	stepper.stepCount = 0;
	stepper.isEnabled = FALSE;
	stepper.direction = 0;
	stepper.flag = 0;

	findHome();					// Zero position of stepper arm
}

void goToStepPosition(int setPos){
	if(setPos > CLICKS_PER_WRAP)
		if(stepper.position >= setPos){
			_StepDirBackward();
			while(stepper.position >= setPos){
				stepper.isEnabled = TRUE;
			}
		}
		else if(stepper.position <= setPos){
			_StepDirForward();
			while(stepper.position <= setPos){
				stepper.isEnabled = TRUE;
			}
		}
	stepper.isEnabled = FALSE;
}//goToStepPosition()


int findHome(void)
{
	volatile unsigned int n;
	_StepDirBackward();

	stepper.isEnabled = TRUE;

	for(n=0;n<80;n++){
		while(P1IN & STEPPER_LIMIT)
		{
			// While stepper limit not hit
		}
	}//for debounce loop

	stepper.isEnabled = FALSE;
	stepper.position = 0;
	_StepDirForward();

	return 1;

}//findHome()


// Timer A0 interrupt service routine
// Clock signal for stepper motor
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A00 (void)
{

	if(stepper.isEnabled)
		P1OUT |= STEPPER_EN;		// Enable Motor
	else
		P1OUT &= ~STEPPER_EN;		// Disable Motor

	if(stepper.isEnabled){
		stepper.stepCount++;
		if(stepper.stepCount > STEPS_PER_CLICK){
			stepper.position += stepper.direction;	// Inc/Dec stepper position counter
			stepper.stepCount = 0;					// Reset step counter
		}
	}
}
