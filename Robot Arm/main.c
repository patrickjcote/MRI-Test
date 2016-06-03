// Setup  --------------------------------------------------------------------- 

// Includes
#include "robotarm.h"

// Global Variables
    // Set all servos to neutral position
    volatile int servoPosition[4] = {1500, 1500, 1500, 1500};

//----------------------------------------------------------------------------- 


    // Testing github for CCS
    //test 2

    // Test 3, commit and push from 6

int main(void)
{

    initialize();                   // Initialize registers

	__bis_SR_register(LPM3 + GIE);  // Global Interrupt Enable, Low power mode

}//main()


void initialize(){

	WDTCTL = WDTPW + WDTHOLD;        // Stop Watchdog Timer


	//--------------------- Serial Comm Setup -------------------------
	if (CALBC1_1MHZ==0xFF)           // If calibration constant erased
	{
		while(1);                    // do not load, trap CPU!!
	}
	DCOCTL = 0;                      // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ;           // Set DCO
	DCOCTL = CALDCO_1MHZ;
	P1SEL = BIT1 + BIT2 ;            // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2 ;           // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;            // SMCLK
	UCA0BR0 = 104;                   // 1MHz 9600
	UCA0BR1 = 0;                     // 1MHz 9600
	UCA0MCTL = UCBRS0;               // Modulation UCBRSx = 1
	UCA0CTL1 &= ~UCSWRST;            // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                 // Enable USCI_A0 RX interrupt

	//---------------------- Servo PWM Setup -----------------------------
	//	TA1CCR0 Controls PWM period for servo control
	//	TA1CCR1 Controls duty cycle for motors 1 + 2
	//	TA1CCR2 Controls duty cycle for motors 3 + 4
	//	Motor 0 on P2.1, Motor 1 on 2.2
	//	Motor 2 on P2.4, Motor 3 on 2.5

	P2DIR = 0xFF;							// P2 all output
	P2OUT = 0x00; 							// Clear all outputs P2

	P2SEL = BIT1 | BIT4;                    // TA1 output on Bit1 and Bit4

	TA1CCR0 = PWM_PERIOD;                   // PWM Period on TA1 of 20ms
	TA1CTL = TASSEL_2 + MC_1;               // SMCLK, up mode
	TA1CCTL0 |= CCIE;						// Compare/Control Interrupt Enable

	TA1CCTL1 = OUTMOD_7 + CCIE;             // CCR1 reset/set, Interrupt Enable
	TA1CCTL2 = OUTMOD_7 + CCIE;             // CCR2 reset/set, Interrupt Enable


}//initialize()

//main.c
