#include <msp430.h>
#include <math.h>
#include <float.h>
#include "reels.h"
#include "i2c.h"



void initReel(){

	//LED Indicator and MCP Reset
	P2DIR |= BIT5 + BIT6;
	P2OUT |= BIT5 + BIT6;

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
	//PWM Out Init
	TA1CCR0 = 40000;			// PWM Period
	TA1CCR1 = 0;
	TA1CCR2 = 0;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	//PWM Outputs
	P2DIR |= BIT4;		// Motor Control P2.4
	P2SEL |= BIT4;
	//PWM IN
	pwmread=TA0R;
	CCR0 = 50000;
	TACTL = TASSEL_2 + MC_2+ID_3;
	P1IE |= BIT5;				// P2.0 interrupt enabled
	P1IES &= ~BIT5;				// P2.0 Hi/lo edge
	P1IFG &= ~BIT5;				// P2.0 IFG cleared
	//Init Globals
	cur_reel_depth = 0;
	set_reel_depth = 0;
	reel_dir = 0;
	reel_flag = 0;
	raster_dir = 1;
	stepper_flag = 0;
	turns = 0;
	pwm_pullup_flag = 0;
	status_code = 0;
	interrupt_code = 0;
	ALL_STOP_FLAG = 1;

	__bis_SR_register(GIE);

}//init_reel()

int goToClick(int setClick){

	//clicks timeout check
	timeout_count1++;
	if(timeout_count1 > REEL_TIMEOUT_1){
		timeout_count1 = 0;
		timeout_count2++;
	}
	if(timeout_count2 > REEL_TIMEOUT){
		ALL_STOP_FLAG = 1;
		return 3;		//Clicks missed, return timeout status
	}

	//determine reel direction
	if(cur_reel_depth != setClick && (P1IN & BIT4)){
		if(cur_reel_depth > setClick){
			reel_dir = 1;
			TA1CCR2 = MOTOR_DOWN;
			return reel_dir;
		}
		if(cur_reel_depth < setClick){
			reel_dir = 2;
			TA1CCR2 = MOTOR_UP;
			return reel_dir;
		}
	}
	else{
		reel_dir = 0;
		reel_flag = 0;
		TA1CCR1=PWM_NEU;
		TA1CCR2 = PWM_NEU;
		ALL_STOP_FLAG = 1;
	}
	if(P1IN & BIT4)
		return 0;
	else
		interrupt_code = 2;
	return 5;

}//goToClick()

void writeStepper(char firstByte, char secondByte){
	char i2cbuf[12];
	i2cbuf[0] = STEPPER_ADDR;
	i2cbuf[1]= ' ';
	i2cbuf[2] = firstByte;
	i2cbuf[3] = secondByte;
	i2c_rx_bb(i2cbuf,4,0);
}

void readStepper(char *stepper_return, char commandByte){
	char i2cbuf[12];
	i2cbuf[0]=STEPPER_ADDR;
	i2cbuf[1]= ' ';
	i2cbuf[2]= commandByte;
	i2c_rx_bb(i2cbuf,3,0);

	i2cbuf[0]=(STEPPER_ADDR+1);
	i2c_rx_bb(i2cbuf,1,3);
	stepper_return[0]=i2cbuf[1];
	stepper_return[1]=i2cbuf[2];
	stepper_return[2]=i2cbuf[3];
}


// --------------------------------  Interrupts ----------------------
// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IFG & BIT4){
		// Hardware interrupt for limit switch
		// Check if reeling up and if current depth is close enough to 0
		if(reel_dir == 2 && reel_flag && cur_reel_depth < LIMIT_SWITCH_MIN){
			cur_reel_depth = 0;
			reel_dir = 0;
			reel_flag = 0;
			status_code = 0;
			interrupt_code = 1;  //Limit switch hit
			if(pwm_pullup_flag)
				interrupt_code = 0;
		}
		else{
			reel_flag = 0;
			interrupt_code = 2; // Limit switch error
		}

		ALL_STOP_FLAG = 1;
		P1IFG &= ~BIT4;
	}//if limit switch

	if(P1IFG & BIT5){
	//PWM In read - pull up reels if signal drops below neutral
		//Read PWM edges
		if (P1IN&BIT5){		//Positive Edge
			if (TA0R>(0xFFFF-4000)){
				pwmread-=TA0R;
				TA0R=0;
			}
			pwmread=TA0R;
			P1IES |= BIT5;
		}
		else{				//Negative Edge
			pwmval=TA0R-pwmread;

			P1IES &=~ BIT5;
		}

	//Compare pwm signal to neutral
		if(pwmval < PWM_NEU){
			set_reel_depth= -10;
			reel_flag = 1;
			reel_dir = 1;
			ALL_STOP_FLAG = 0;
			pwm_pullup_flag = 1;
			timeout_count1 = 0;
			timeout_count2 = 0;
		}
		else if(pwm_pullup_flag){
			reel_flag = 0;
			interrupt_code = 0;
			set_reel_depth = cur_reel_depth;
			ALL_STOP_FLAG = 1;
			pwm_pullup_flag = 0;
		}

		P1IFG &= ~BIT5;
	}//if PWM In read
}

// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

	//Hardware interrupt for click counter
	if(reel_dir == 2 && reel_flag){
		cur_reel_depth++;
		stepper_flag = 0;
	}
	if(reel_dir == 1 && reel_flag){
		cur_reel_depth--;
		stepper_flag = 1;
		turns += raster_dir;
	}
	if(cur_reel_depth > MAX_CLICKS){
		ALL_STOP_FLAG = 1;
		reel_flag = 0;
		interrupt_code = 3; //Clicks out of bounds
	}
	if(cur_reel_depth < MIN_CLICKS)
		cur_reel_depth = 0;

	//Click hit, reset timeout counter
	timeout_count1 = 0;
	timeout_count2 = 0;

	P2IFG &= ~BIT0;			// Clear interrupt

}
