#include <msp430.h>
#include "serial_handler.h"

void ADC_init(void);
int ADC_val(void);
int pump_water(int, int);
void pump_init(void);
void arm_init(void);
void next_bottle(void);
int reset_position(void);

/*
 * main.c
 */
//#define ARMP BIT0		//Define servo arm power
//#define ARMD BIT1		//Define servo arm direction of rotation
//#define ARML BIT2		//Define Servo LED for photogate
//#define ARMG BIT3		// Define servo arm photogate input
//#define coffset 100		//define current value to exceed for shutting off motor

#define ARMF	BIT0	//Define arm movement direction forward (P2.0)
#define ARMR	BIT1	//Define arm movement direction Reverse (P2.1)
#define ARMPG	BIT2	//Define arm photogate (P2.2)
#define ARMCS	BIT0	//Define arm current sensor (P1.0)
#define PUMPP	BIT3	//Define pump LED and pump on bit
#define PUMPD	BIT4	//Define pump direction bit
#define PUMPG	BIT5	//Define pump photogate
#define pumpscale  100	//Define number of deciliters per click of the pump

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	volatile char identify[]="Pump_and_Sampler";
	volatile int n,pump_dir,pump_vol,pump_flag=1,arm_flag,bottle;
	ADC_init();
	__delay_cycles(50000);
	i2c_slave_init(0x48);  //Set slave address to 0x48
	uart_init(4);   // set uart baud rate to 9600
	pump_init();
	arm_init();
	bottle = reset_position();


	__bis_SR_register(GIE);
	i2cTXData[0]=3;			//Dummy values first input to i2cTXData
	i2cTXData[1]=10;
	i2cTXData[2]=12;
	while(1){
		if (eos_flag){					// if data received from the UART

			switch(rx_data_str[0]){
			case 'I':
				for (n=0;n<9;n++)
					tx_data_str[n]=identify[n];
				uart_write_string(0,9);
				break;

			case 'P':							// purge
				if (pump_flag){
					arm_flag = 0;
					pump_dir = 0;

					if (rx_flag < 5)
						break;
					//					pump_vol = ((rx_data_str[2] - 0x30) * 100);
					//					pump_vol += ((rx_data_str[2] - 0x30) * 10);
					//					pump_vol += (rx_data_str[3] - 0x30);
					pump_vol = 99;
					pump_water(pump_dir, pump_vol);
				}
				arm_flag = 1;
				break;

			case 'F':							// fill
				if (pump_flag){
					arm_flag = 0;
					pump_dir = 1;

					if (rx_flag < 5)
						break;
					//					pump_vol = ((rx_data_str[2] - 0x30) * 100);
					//					pump_vol += ((rx_data_str[2] - 0x30) * 10);
					//					pump_vol += (rx_data_str[3] - 0x30);
					pump_vol = 99;
					pump_water(pump_dir, pump_vol);
				}
				arm_flag = 1;
				break;

			case 'N':							// next bottle
				if (arm_flag){
					pump_flag = 0;
					if (bottle < 24){
						next_bottle();
						bottle +=1;
					}
					else if (bottle >= 24){
						bottle = reset_position();
					}
					tx_data_str = bottle;
					uart_write_string(0,1);
				}
				pump_flag = 1;
				break;
			}
			rx_flag=0;
			eos_flag=0;

		}
		if (i2crxflag){					// if data received as the I2C slave
			__delay_cycles(50000);
			i2crxflag=0;

		}



	}
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	__bic_SR_register_on_exit(CPUOFF); // Clear CPUOFF bit from 0(SR)
}

//Function enables the pump and servo arm control


////Function to set sampler to set bottle
//int set_bottle(int BotNum, int current){
//	int gstate,curval,curstate;
//	//make sure motor is off
//	P2OUT&=~ARMP;
//	P2OUT&=~ARML;
//	// If BotNum<1 rotate negative else rotate positive:  set direction
//	P2OUT&=~ARMD;
//	if (BotNum>current)
//		P2OUT|=ARMD;
//	// turn on motor
//	P2OUT|=(ARML+ARMP);
//	gstate=1;
//	if (!(P2IN&ARMG)){
//		gstate=0;		// gate is not blocked
//	}
//	curstate=gstate;
//	while (current!=BotNum){
//		//check to see if a stop value has been issued
//		// check to see if the photogate is blocked
//		if (!(P2IN&ARMG))
//			curstate=0;		// gate is not blocked
//		else
//			curstate=1;
//
//		if (curstate!=gstate){
//			//Delay for a bit for debounce
//			gstate=curstate;		//Set new state
//			if (gstate){			//When gate blocked set new bottle number
//				if(current<BotNum)
//					current++;
//				if(current<BotNum)
//					current--;
//			}
//		}
//		if (gstate==0){
//			// if not blocked check the current being drawn
//			ADC10CTL0 |= ENC + ADC10SC;
//			while (ADC10CTL1 & ADC10BUSY);          // ADC10BUSY?
//			curval= ADC10MEM;
//			if ((curval>(512+coffset))||(curval<(512-coffset))){
//				P2OUT&=~(ARMP+ARML);  //Turn off motor
//				P2OUT^=ARMD; 			// swap direction of motor
//				// pause for a bit
//				P2OUT|=(ARMP+ARML);		//back off the motor
//				while(!(ARMG&P2IN));
//				P2OUT&=~(ARMP+ARML);	//Check to see if the photogate is blocked before turning off the motor
//				if (BotNum>current){
//					current=24;
//					return 24;
//				}
//				current=1;
//				return 1;
//
//			}
//
//		}
//
//	}
//	return current;		//return value that we set the sampler to.
//
//}



//Function to pump N deciliters of water in one direction

void ADC_init(){
	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_0; // input A1
	ADC10AE0 |= BIT0; // PA.0 ADC option select
}

int ADC_val(void){
	volatile int CS_out;
	ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE); // LPM0, ADC10_ISR will force exit
	CS_out = ADC10MEM;
	return(CS_val);
}

int pump_water(int direction, int deciliters){
	int gstate,curstate;
	P1OUT&=~PUMPP;		//turn off pump and LED
	P1OUT|=PUMPD;
	if (direction)
		P1OUT&=~PUMPD;
	P1OUT|=PUMPP;
	gstate=1;
	if (!(P1IN&PUMPG)){
		gstate=0;
	}
	curstate=gstate;
	while (deciliters>0){												//{Counter to make sure pump is moving}
		// Check to see if a stop value has been issued
		if ((P1IN&PUMPG)==0x00)
			curstate=0;		// gate is not blocked
		else
			curstate=1;

		if (curstate!=gstate){
			__delay_cycles(50000);//Delay for a bit for debounce
			gstate=curstate;		//Set new state
			if (gstate)
				deciliters--;
		}

	}
	P1OUT&=~PUMPP;		//turn off pump and LED
	return 0;


}

void pump_init(void){

	P1DIR |= PUMPP + PUMPD;					//Set outputs for direction and pump on
	P1DIR &=~ PUMPG;						//Reads the photogate
	P1OUT &=~ PUMPP;						//clear the input to the pump

}

void arm_init(void){

	P2DIR |= ARMF + ARMR;			// set ports 2.0,2.1 as outputs and 2.2 as input
	P2REN |= ARMPG;					// enable pull down resistors for ports 2.2
	P2OUT &=~ ARMF + ARMR + ARMPG;	// set ports 2.1-2.5 low
}

void next_bottle(void){

	P2OUT &=~ ARMF + ARMR;			// clear 2.0 & 2.1 (motor off)
	P2OUT |= AEMF;			// set 2.0 high to rotate arm forward
	__delay_cycles(50000);	// delay for a bit

	CS_val = ADC_val();
	if (CS_val > 700){		// check input at 2.2 (end current sensor)
		reset_position();			// call the position reset function
		break;
	}

	while((P2IN & 0x20) != 0x20){	// check input at 2.5 to see if next bottle is reached (photogate)
		P2OUT |= 0x02;				// set 2.1 high to rotate arm forward
	}
	P2OUT &=~ 0x0A;					// clear 2.1 & 2.3 (motor off)

	return;
}

int reset_position(void){

	P2OUT &=~ 0x0A;					// clear 2.1 & 2.3 (motor off)
	while((P2IN & 0x10) != 0x10){	// check input at 2.4 (start current sensor)
		P2OUT |= 0x08;				// set 2.3 high to rotate arm backwards
	}
	P2OUT &=~ 0x0A;					// clear 2.1 & 2.3 (motor off)
	while((P2IN & 0x20) != 0x20){	// check input at 2.5 to see if next bottle is reached (photogate)
		P2OUT |= 0x02;				// set 2.1 high to rotate arm forward
	}
	P2OUT &=~ 0x0A;					// clear 2.1 & 2.3 (motor off)
	bottle = 1;

	return(bottle);
}
