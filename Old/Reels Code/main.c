#include <msp430.h>
#include "reels.h"

volatile int cur_reel_depth, reel_dir, set_reel_depth, ALL_STOP_FLAG=1, reel_flag=0, set_reel_level;


int main(void) {

	volatile int n, k;
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);

	__bis_SR_register(GIE);  // Enable interrupts (yes this is needed)
	__delay_cycles(50000);

	initReel();

	while(1){
		if(reel_flag)
		{
			goToClick(set_reel_depth);

		}

	}
	return 0;
}//main()


void initReel(){

	//Limit Switch input
	P1DIR &= ~BIT4;				// Limit switch input on 1.4
	P1IE |=  BIT4;				// 1.4 interrupt enabled
	P1IES |= BIT4;				// 1.4 Hi/lo edge
	P1REN |= BIT4;				// Enable Pull Up (P1.4)
	P1IFG &= ~BIT4;				// P1.4 IFG clear
	//Click Count input
	P2DIR &= ~BIT0;				// Click count input on 2.0
	P2IE |=  BIT0;				// P2.0 interrupt enabled
	P2IES |= BIT0;				// P2.0 Hi/lo edge
	P2REN |= BIT0;				// Enable Pull Up (P2.0)
	P2IFG &= ~BIT0;				// P2.0 IFG clear
	//PWM Init
	TA1CCR0 = 40000;
	TA1CCR1 = 0;
	TA1CCR2 = 0;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	//PWM Outputs
	P2DIR |= BIT4;				//Motor Control on P2.4
	P2SEL |= BIT4;				//TA1.2 Output on P2.4
	P2DIR |= BIT2;				// P2.2 Actuator PWM
	P2SEL |= BIT2;				//TA1.1 Output to  P2.2
	//Init Globals
	cur_reel_depth = 0;
	set_reel_depth = 0;
	reel_dir = 0;
	ALL_STOP_FLAG = 0;

	__bis_SR_register(GIE);

}//init_reel()

int goToClick(int setClick){

	if(cur_reel_depth != setClick){
		if(cur_reel_depth > setClick){
			reel_dir = 1;
			set_reel_level = 3;
			TA1CCR2 = PWM_MIN;
			setReelLevel();
			return 1;
		}
		if(cur_reel_depth < setClick){
			reel_dir = 2;
			TA1CCR2 = PWM_MAX;
			set_reel_level = 2;
			setReelLevel();
			return 2;
		}
	}
	else{
		reel_dir = 0;
		TA1CCR2 = PWM_NEU;
		set_reel_level = 0;
		setReelLevel();
		return 0;
	}
}//goToClick()

int setReelLevel(){
	if(set_reel_level == 3){
		volatile int currentWrap;
		currentWrap = (cur_reel_depth / TURNS_PER_WRAP)+1;

		if(currentWrap % 2)
			TA1CCR1=PWM_MIN;
		else
			TA1CCR1=PWM_MAX;
		return 1;
	}
	if(set_reel_level == 2){
		TA1CCR1 = PWM_MIN;
	}
	if(set_reel_level == 1){
		TA1CCR1 = PWM_MAX;
	}
	if(set_reel_level == 0 || ALL_STOP_FLAG == 1)
	{
		TA1CCR1 == 0;
	}

}

int conv_char_hex(char *in_str,int num){
	volatile int k,tempc=0;
	for (k=0;k<num;k++){
		tempc<<=4;
		if (in_str[k]>0x60)
			tempc+=in_str[k]-87;
		else if (in_str[k]>0x40)
			tempc+=in_str[k]-55;
		else
			tempc+=in_str[k]-0x30;

	}
	return tempc;
}



// --------------------------------  Interrupts ----------------------
// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	//Hardware interrupt for limit switch
	cur_reel_depth = 0;
	ALL_STOP_FLAG = 1;
	P1IFG &= ~BIT4;
}

// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	//Hardware interrupt for click counter
	if(reel_dir == 2)
		cur_reel_depth++;
	if(reel_dir == 1)
		cur_reel_depth--;

	P2IFG &= ~BIT0;
}
