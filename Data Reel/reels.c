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
	TA1CCR0 = 40000;
	TA1CCR1 = 0;
	TA1CCR2 = 0;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	//PWM Outputs
	P2DIR |= BIT2 + BIT4;		// Motor Control P2.4 | Actuator P2.2
	P2SEL |= BIT2 + BIT4;
	//PWM IN
	pwmread=TA0R;
	CCR0 = 50000;
	TACTL = TASSEL_2 + MC_2+ID_3;
	P1IE |= BIT5;				// P2.0 interrupt enabled
	P1IES &= ~BIT5;				// P2.0 Hi/lo edge
	P1IFG &= ~BIT5;				// P2.0 IFG cleared
	//Level Timer Interrupt
	TA1CCTL0 = CCIE;
	//Init Globals
	cur_reel_depth = 0;
	set_reel_depth = 0;
	reel_dir = 0;
	reel_flag = 0;
	pu_flag = 0;
	status_code = 0;
	interrupt_code = 0;
	set_angle = 0;
	avg_pointer = 0;
	autolevel_flag = 0;
	get_level_flag = 0;
	lvl_compare = 0;
	ALL_STOP_FLAG = 1;

	__bis_SR_register(GIE);

}//init_reel()

int goToClick(int setClick){

	timeout_count1++;
	if(timeout_count1 > REEL_TIMEOUT_1){
		timeout_count1 = 0;
		timeout_count2++;
	}
	if(timeout_count2 > REEL_TIMEOUT){
		ALL_STOP_FLAG = 1;
		return 3;
	}

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
		autolevel_flag = 0;
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


void autoLevel(){
	volatile int currentWrap;

	currentWrap = (cur_reel_depth / TURNS_PER_WRAP)+1+TOP_WRAP_ANGLE;
	if(autolevel_flag == 2){
		if(currentWrap % 2)
			set_angle = +REELING_ANGLE;
		else
			set_angle = -REELING_ANGLE;
	}

	if(average > (set_angle + ANGLE_TOLERANCE)){
		TA1CCR1=PWM_MIN;
	}
	else if(average < (set_angle - ANGLE_TOLERANCE)){
		TA1CCR1=PWM_MAX;
	}
	else
		TA1CCR1=PWM_NEU;
}

void getLevel(){

	int accelvec[3], sum, i;
	read_ADXL(accelvec);

	// Load data to return if asked for via I2C Slave
	TXData[0]=13;
	TXData[2]=(accelvec[0]&0xFF0);
	TXData[1]=((accelvec[0]>>8)&0xFF);
	TXData[4]=(accelvec[1]&0xFF0);
	TXData[3]=((accelvec[1]>>8)&0xFF);

	current_angle = atan((float)accelvec[0]/(float)accelvec[1]);

	current_angle *= 180/PI; //rads to degrees

	if( avg_pointer > (SAMPLES-1) ){
		avg_pointer = 0;
	}

	avg_angle[avg_pointer] = (int)(current_angle*100);
	avg_pointer++;
	sum = 0;

	for(i=0;i<SAMPLES;i++){
		sum += avg_angle[i];
	}

	average = ((float)sum/(float)SAMPLES)/(float)100;

}//autoLevel


void init_ADXL(void){
	char i2cbuf[12];
	i2cbuf[0]=ADXL_ADDR;
	i2cbuf[1]=0x2D;		//Power control address
	i2cbuf[2]=0x08;		//Turn on the power control
	i2c_rx_bb(i2cbuf,3,0);		//Transmit the power on sequence
	i2cbuf[0]=ADXL_ADDR;
	i2cbuf[1]=0x2C;
}

void read_ADXL(int *accel_vec){
	char i2cbuf[12];
	i2cbuf[0]=ADXL_ADDR;
	i2cbuf[1]=0x32+0x80;
	i2c_rx_bb(i2cbuf,2,0);

	i2cbuf[0]=(ADXL_ADDR+1);

	i2c_rx_bb(i2cbuf,1,6);
	accel_vec[0]=(i2cbuf[1]+((i2cbuf[2]<<8)));
	accel_vec[1]=(i2cbuf[3]+((i2cbuf[4]<<8)));
	accel_vec[2]=(i2cbuf[5]+((i2cbuf[6]<<8)));
}


// --------------------------------  Interrupts ----------------------
// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if(P1IFG & BIT4){
		//Hardware interrupt for limit switch
		if(reel_dir && reel_flag && cur_reel_depth < LIMIT_SWITCH_MIN){
			cur_reel_depth = 0;
			reel_dir = 0;
			reel_flag = 0;
			status_code = 0;
			interrupt_code = 1;  //Limit switch hit
			if(pu_flag)
				interrupt_code = 0;
		}
		else{
			reel_flag = 0;
			interrupt_code = 2; // Limit switch error
		}

		ALL_STOP_FLAG = 1;
		P1IFG &= ~BIT4;
	}//if limit switch
	if(P1IFG & BIT5){ //PWM In read

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

		if(pwmval < PWM_NEU){
			set_reel_depth= -10;
			reel_flag = 1;
			reel_dir = 1;
			ALL_STOP_FLAG = 0;
			pu_flag = 1;
			timeout_count1 = 0;
			timeout_count2 = 0;
		}
		else if(pu_flag){
			reel_flag = 0;
			interrupt_code = 0;
			set_reel_depth = cur_reel_depth;
			ALL_STOP_FLAG = 1;
			pu_flag = 0;
		}

		P1IFG &= ~BIT5;
	}//if PWM In read
}

// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

	//Hardware interrupt for click counter
	if(reel_dir == 2 && reel_flag)
		cur_reel_depth++;
	if(reel_dir == 1 && reel_flag)
		cur_reel_depth--;
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

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{


	get_level_flag = 1;

	TA1CCTL0 &= ~CCIFG;

}
