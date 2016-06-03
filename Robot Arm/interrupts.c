#include "robotarm.h"

// ---------- UART Interrupt --------------------------
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{

	volatile char inChar;
	inChar = UCA0RXBUF;		// Read in UART serial port
	testInput(inChar);		// Test the input

}//UART_ISR()




// ------- Timer A1.0 ISR ------------------------------
//
// Timer_A 1.0 is the period for the PWM
// This ISR sets up the first duty cycle
// Outmode_7 sets output HIGH until CCRn is hit.
//
// -----------------------------------------------------
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMERA1_ISR(void)
{
	TA1CCTL1 = OUTMOD_7 + CCIE;		// A1.1 Output Mode and Enable Interrupt
	TA1CCTL2 = OUTMOD_7 + CCIE;		// A1.2 Output Mode and Enable Interrupt
	P2SEL = BIT1 + BIT4;			// Timer A Output on Bit 1 and Bit 4
	TA1CCR1 = servoPosition[0];		// Motor 0 duty cycle
	TA1CCR2 = servoPosition[2];		// Motor 2 duty cycle

}//timerA1.0 interrupt()



// ------- Timer_A 1.1 + 1.2 ISR ------------------------------
//
// This ISR sets up the second duty cycle
// A1 Interrupt Vector (TAIV) = 2 for A1.1 and 4 for A1.2
// Outmode_1 is low until CCRn is hit
//
// -----------------------------------------------------
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMERA11_ISR(void)
{
	switch(TA1IV){
	case 2: //Timer A1.1 Interrupt
		TA1CCTL1 = OUTMOD_1;
		P2SEL &= ~BIT1;		// Stop output of Timer A1.1 on Pin 2.1
		P2SEL |= BIT2;		// TA1.1 Output on Pin 2.2 
		TA1CCR1 = PWM_PERIOD - servoPosition[1];	// Motor 1 duty cycle
		break;
	case 4: //Timer A1.2 Interrupt
		TA1CCTL2 = OUTMOD_1;
		P2SEL &= ~BIT4;		// Stop output of Timer A1.2 on Pin 2.4
		P2SEL |= BIT5;		// TA1.2 Output on P2.5
		TA1CCR2 = PWM_PERIOD - servoPosition[3];	// Motor 3 duty cycle
		break;
	default:
		break;
	}

}//timerA1.1+2 interrupt()
