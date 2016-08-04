#include <msp430.h> 

/*
 * main.c
 */
#define FORWARD 1
#define BACKWARD -1
#define RASTER_FORWARD 1
#define RASTER_BACKWARD 0
#define MAX_STEPS 13000
#define MIN_STEPS 10

void setDirection();
int findHome(void);
volatile int stepper_pos, stepper_dir, stepper_en, raster_dir;

int main(void) {

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	BCSCTL1 = CALBC1_16MHZ;		// Set Clock Speed
	DCOCTL = CALDCO_16MHZ;

	// Dir + Enable
	P1DIR |= BIT3 + BIT4;
	P1OUT |= BIT3 + BIT4;

	// Stepper PWM
	P1DIR |= BIT2;
	P1SEL |= BIT2;
	TA0CCR0 = 16000;			// 1kHz
	TA0CCR1 = 1600;				// 10% duty
	TA0CCTL1 = OUTMOD_7;
	TA0CCTL0 = CCIE;			// Interrupt Enable
	TA0CTL = TASSEL_2 + MC_1;	// smclock and count up

	// Stepper Limit Switch
	P1DIR &= ~BIT5;				// Limit switch input on 2.1
	P1REN |= BIT5;				// Enable Pull Up (P1.4)

	stepper_pos = 0;
	stepper_dir = BACKWARD;
	stepper_en = 1;

	__delay_cycles(50000);
	__bis_SR_register(GIE);

	findHome();

	while(1){
		stepper_en = 1;
		if(raster_dir){
			setDirection(FORWARD);
			if(stepper_pos > MAX_STEPS)
				raster_dir = RASTER_BACKWARD;
		}
		else{
			setDirection(BACKWARD);
			if(stepper_pos < MIN_STEPS)
				raster_dir = RASTER_FORWARD;
		}

	}
	return 0;

}

void setDirection(int direction){

	volatile int n;
	stepper_dir = direction;

	if(direction == FORWARD)
		P1OUT &= ~BIT3;		// Set Direction
	if(direction == BACKWARD)
		P1OUT |= BIT3;		// Set Direction

	for(n=0;n<100;n++);		// ~5us delay


}// setDirection()

int findHome(void)
{
	setDirection(BACKWARD);

	while(P1IN & BIT5)
	{
		stepper_dir = BACKWARD;
	}

	stepper_pos = 0;
	setDirection(FORWARD);
	stepper_en = 0;
	raster_dir = RASTER_FORWARD;
	return 1;

}// findHome()


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
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
