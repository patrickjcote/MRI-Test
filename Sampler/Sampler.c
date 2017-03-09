#include <msp430.h>
#include "sampler.h"

/*Variables initialization:
 *CS_val: ADC value of current sensor input voltage, clicks: number of pump revolutions per specified volume
 *c: delay multiplier, PPG: pump photogate positive edge transition check, APG: arm photogate positive edge transition check
 */
volatile int CS_val,clicks,c,PPG,APG;

/*ADC peripheral initializing function*/
void ADC_init(){
	ADC10CTL0 = ADC10SHT_2 + ADC10ON;	//ADC10-Control0 = sample time(number of sampling period clock cycles) + turn on ADC10
	ADC10CTL1 = INCH_0; 				//ADC10-control1 = input channel 0 (P1.0)
	ADC10AE0 |= BIT0; 					//ADC input enable register 0
}

/*ADC value reading function*/
int ADC_val(void){
	ADC10CTL0 |= ENC + ADC10SC; 		//ADC10-Control0 = Enable conversion + Start conversion on ADC10
	while(ADC10CTL1 & ADC10BUSY);		//to check for ADC10 EOC (End of Conversion)
	CS_val = ADC10MEM;					//current sensor output = value stored in ADC memory
	return CS_val;						//return the current sensor output value to the calling function
}

/*Sampler initialization function*/
void sampler_init(void){

	P1DIR |= PUMP + PUMPD;				//set outputs for direction and pump
	P1OUT &=~ PUMP + PUMPD;				//set outputs to pump low (pump off)
	P2DIR |= ARMF + ARMR;				//set outputs for arm movement direction
	P2OUT |= ARMF + ARMR;				//set outputs to arm high (arm off)
	P2DIR |= BIT5 + BIT6;				//set outputs for board LED and board VSP reset
	P2OUT |= BIT5 + BIT6;				//set outputs to board high
}

/*Water sampling function - takes 2 input values: pump direction & sample volume*/
void pump_water(int pump_dir, int pump_vol){

	if(!(P1OUT & PUMP)){				//check if pump is off
		P1OUT &=~ PUMP;					//clear output to pump
		if(pump_vol > 800)				//check if sample volume is greater than 800ml
			pump_vol = 800;				//limit max sample volume to 800ml to avoid overflow

		clicks = pump_vol * volscale;	//pump revolutions corresponding to desired sample volume

		if (pump_dir)					//check desired pump direction - if forward (1):
			P1OUT |= PUMPD;				//sample
		else							//else reverse(0)
			P1OUT &=~ PUMPD;			//clear tube

		__delay_cycles(50000);			//delay
		P1OUT |= PUMP;					//turn pump on (begin sampling)
		__delay_cycles(50000);			//delay
	}


	if (clicks > 0){					//if number of clicks is greater than 0 execute the following code

		if (!(P1IN & PUMPG))			//check if pump photogate is not blocked
			PPG = 1;					//pump photogate transition check = 1 (set)
		if((P1IN & PUMPG) && PPG){		//check if pump photogate is blocked and pump photogate transition is set
			clicks--;					//decrement number of clicks
			PPG = 0;					//pump photogate transition check = 0 (clear)
		}
	}

	if(clicks <= 0){					//if number of clicks is less than or equal 0 execute the following code
		P1OUT &=~ PUMP;					//clear output to pump (pump off)
		pump_flag = 0;					//set pump flag = 0
		status = 0;						//status = 0 - Sampler ready
	}

	return;
}

/*Bottle selection function - takes one input value: desired bottle number*/
void arm_position(int new_bottle){
	CS_val = ADC_val();					//call the ADC function to obtain the output value of the current sensor
	ARM_TO--;							//decrement the value of the arm timeout

	//if the ADC value of CS output exceeds 680 AND arm is moving forward AND arm timeout is less than or equal 0, do the following
	if((CS_val > 680) && !(P2OUT & ARMF) && (ARM_TO <= 0)){
		P2OUT |= ARMF + ARMR;			//set arm outputs high (stop arm)
		P2OUT &=~ ARMR;					//set arm reverse low (move arm backwards)
		for (c=0;c<75;c++)				//delay multiplier of 75
			__delay_cycles(50000);		//delay (to eliminate effect of initial current spike and to correctly place gear-moving rod)
		P2OUT |= ARMF + ARMR;			//set arm outputs high (stop arm)
		APG = 0;						//arm photogate transition check = 0 (clear)
		cur_bottle = 24;				//current bottle = 24
		new_bottle = cur_bottle;		//make new bottle = current bottle (to activate stop condition)
	}

	//if the ADC value of CS output exceeds 680 AND arm is moving backwards AND arm timeout is less than or equal 0, do the following
	if((CS_val > 680) && !(P2OUT & ARMR) && (ARM_TO <= 0)){
		P2OUT |= ARMF + ARMR;			//set arm outputs high (stop arm)
		P2OUT &=~ ARMF;					//set arm forward low (move arm forwards)
		for (c=0;c<75;c++)				//delay multiplier of 75
			__delay_cycles(50000);		//delay (to eliminate effect of initial current spike and correctly place gear-moving rod)
		P2OUT |= ARMF + ARMR;			//set arm outputs high (stop arm)
		APG = 0;						//arm photogate transition check = 0 (clear)
		cur_bottle = 1;					//current bottle = 1
		new_bottle = cur_bottle;		//make new bottle = current bottle (to activate stop condition)
	}

	if (cur_bottle < new_bottle){		//if current bottle number is less than desired bottle
		P2OUT |= ARMR;					//set arm reverse high (reverse movement off)
		P2OUT &=~ ARMF;					//set arm forward low (move arm forwards)
		for (c=0;c<15;c++)				//delay multiplier of 15
			__delay_cycles(50000);		//delay (to eliminate effect of initial current spike)

		if(!(P2IN & ARMPG))				//check if arm photogate is not blocked
			APG = 1;					//arm photogate transition check = 1 (set)
		if((P2IN & ARMPG) && APG){		//check if arm photogate is blocked AND arm photogate transition check is set
			cur_bottle++;				//increment current bottle number
			ARM_TO = 30;				//set arm timeout = 30
			APG = 0;					//arm photogate transition check = 0 (clear)
		}

	}

	if (cur_bottle > new_bottle){		//if current bottle number is greater than desired bottle
		P2OUT |= ARMF;					//set arm forward high (forward movement off)
		P2OUT &=~ ARMR;					//set arm reverse low (move arm backwards)
		for (c=0;c<15;c++)				//delay multiplier = 15
			__delay_cycles(50000);		//delay (to eliminate effect of initial current spike)

		if(!(P2IN & ARMPG))				//check if arm photogate is not blocked
			APG = 1;					//arm photogate transition check = 1 (set)
		if((P2IN & ARMPG) && APG){		//check if arm photogate is blocked AND arm photogate transition check is set
			cur_bottle--;				//decrement current bottle number
			ARM_TO = 30;				//set arm timeout = 30
			APG = 0;					//arm photogate transition check = 0 (clear)
		}
	}

	if(cur_bottle == new_bottle){		//if current bottle number equals desired bottle number (desired bottle is reached)
		for(c=0;c<45;c++)				//delay multiplier of 45
			__delay_cycles(50000);		//delay (to correctly place gear moving-rod)
		P2OUT |= ARMF + ARMR;			//set arm outputs high (stop arm)
		arm_flag = 0;					//set arm flag = 0
		status = 0;						//status = 0 - Sampler ready
	}

	return;
}

/*Arm position reset function - places arm at bottle# 1*/
void arm_reset(void){
	CS_val = ADC_val();					//call the ADC function to obtain the output value of the current sensor
	ARM_TO--;							//decrement the value of the arm timeout

	if(P2OUT & (ARMF+ARMR)){			//if the arm is not moving execute the following
		P2OUT |= ARMF;					//set arm forward high (forward movement off)
		P2OUT &=~ ARMR;					//set arm reverse low (move arm backwards)
		for(c=0;c<20;c++)				//delay multiplier of 20
			__delay_cycles(50000);		//delay (to eliminate effect of initial current spike)
	}

	if(!(P2IN & ARMPG))					//check if arm photogate is not blocked
		APG = 1;						//arm photogate transition check = 1 (set)
	if((P2IN & ARMPG) && APG){			//check if arm photogate is blocked AND arm photogate transition check is set
		ARM_TO = 30;					//set arm timeout = 30
		APG = 0;						//arm photogate transition check = 0 (clear)
	}

	//if the ADC value of CS output exceeds 680 AND arm timeout is less than or equal 0, do the following
	if((CS_val > 680) && (ARM_TO <= 0)){
		P2OUT |= ARMR;					//set arm reverse high (reverse movement off)
		P2OUT &=~ ARMF;					//set arm forward low (move arm forwards)
		for(c=0;c<75;c++)				//delay multiplier of 75
			__delay_cycles(50000);		//delay (to eliminate effect of initial current spike and to correctly place gear-moving rod)
		P2OUT |= ARMF + ARMR;			//set arm outputs high (stop arm)
		cur_bottle = 1;					//set current bottle number = 1
		arm_reset_flag = 0;				//set arm reset flag = 0
		status = 0;						//status = 0 - Sampler ready
		APG = 0;						//arm photogate transition check = 0 (clear)
	}

	return;
}

