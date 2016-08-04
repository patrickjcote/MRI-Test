#include <msp430.h> 

/*
 * main.c
 */
#define STEP_FORWARD 1
#define STEP_BACKWARD -1
#define RASTER_FORWARD 1
#define RASTER_BACKWARD 0
#define MAX_STEPS 13000
#define MIN_STEPS 10

void setDirection();
void findHome(void);
volatile int stepper_pos, stepper_dir, stepper_en, raster_dir;

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	BCSCTL1 = CALBC1_16MHZ;		// Set Clock Speed
	DCOCTL = CALDCO_16MHZ;

	// Dir + Enable
	P1DIR |= BIT3 + BIT4;
	P1OUT |= BIT3 + BIT4;

	// Stepper PWM
	P2DIR |= BIT2;
	P2SEL |= BIT2;
	TA1CCR0 = 16000;			// 1kHz
	TA1CCR1 = 1600;				// 10% duty
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL0 = CCIE;			// Interrupt Enable
	TA1CTL = TASSEL_2 + MC_1;	// smclock and count up

	// Stepper Limit Switch
	P2DIR &= ~BIT1;				// Limit switch input on 2.1
	P2REN |= BIT1;				// Enable Pull Up (P1.4)

	stepper_pos = 0;
	stepper_dir = STEP_BACKWARD;
	stepper_en = 1;

	__delay_cycles(50000);
	__bis_SR_register(GIE);

	findHome();

	while(1){
		stepper_en = 1;
		if(raster_dir){
			setDirection(STEP_FORWARD);
			if(stepper_pos > MAX_STEPS)
				raster_dir = RASTER_BACKWARD;
		}
		else{
			setDirection(STEP_BACKWARD);
			if(stepper_pos < MIN_STEPS)
				raster_dir = RASTER_FORWARD;
		}

	}
	return 0;

}

void setDirection(int direction){

	volatile int n;
	P2SEL &= ~BIT1;			//Disable steps

	stepper_dir = direction;

	if(direction == STEP_FORWARD)
		P1OUT &= ~BIT3;		// Set Direction
	if(direction == STEP_BACKWARD)
		P1OUT |= BIT3;		// Set Direction

	for(n=0;n<100;n++);		// ~5us delay
	P2SEL |= BIT1;			// Enable steps

}// setDirection()

void findHome(void)
{
	setDirection(STEP_BACKWARD);

	while(P2IN & BIT1)
	{
		stepper_dir = STEP_BACKWARD;
	}

	stepper_pos = 0;
	setDirection(STEP_FORWARD);
	stepper_en = 0;
	raster_dir = RASTER_FORWARD;

}// findHome()

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	// Enable and Direction checks
	if(stepper_en)
		P1OUT |= BIT4;		// Enable Motor
	else
		P1OUT &= ~BIT4;		// Disable Motor


	if(stepper_en){
		stepper_pos += stepper_dir;	// Inc/Dec stepper position
	}
}
