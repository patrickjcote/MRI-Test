/*
 * main.c
 *
 *  Created on: February 9, 2017
 *      Author: Patrick Cote
 *
 */

// Includes
#include <msp430.h>
#include "serial_handler.h"
#include "i2c.h"
#include "reels.h"

// Global Variables
volatile char statusCode, interruptCode, allStopFlag;
struct Reel reel;
struct Stepper stepper;


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;			// Stop watchdog timer

	volatile char identify[]=BOARD_NAME;
	volatile int n, k, ok2send=0;

	__delay_cycles(50000);
	i2c_slave_init(BOARD_ADDRESS); 		//Set slave address to 0x48
	__delay_cycles(50000);
	i2c_init();
	__delay_cycles(50000);
	uart_init(4);						// set uart baud rate to 9600
	__delay_cycles(50000);
	BCSCTL1 = CALBC1_16MHZ;				// Set Clock Speed to 16MHz
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);

	initReel();
	__delay_cycles(50000);
	initStepper();
	__delay_cycles(50000);

	for (k=0;k<200;k++)
		__delay_cycles(50000);

	__bis_SR_register(GIE);

	i2cTXData[0]=3;						//Dummy values first input to i2cTXData
	i2cTXData[1]=10;
	i2cTXData[2]=12;

	while(1){

		// Serial Comm Handling ------------------------------------------------------------------
		if (eos_flag){					// if data received from the UART
			eos_flag=0;
			if (rx_data_str[0]=='I'){	// If it is the identifying character return the device ID
				for (n=0;n<8;n++)
					tx_data_str[n]=identify[n];
				uart_write_string(0,8);
			}
			else{
				ok2send=input_handler(rx_data_str,tx_data_str);
				if (ok2send)
					uart_write_string(0,ok2send);
			}
			rx_flag=0;
		}
		if (i2crxflag){					// if data received as the I2C slave
			__delay_cycles(50000);
			ok2send=input_handler(i2cRXData+1,i2cTXData);
			i2crxflag=0;
		}

		// Operation Flag Handling -----------------------------------------------------------------
		if (!allStopFlag){
			if(reel.flag){
				statusCode = goToClick();
			}
			if(stepper.flag){
				stepper.isEnabled = TRUE;
			}
		}//if no allStopFlag

		if (allStopFlag){
			TA1CCR2 = 0;
			reel.flag = 0;
			reel.direction = 0;
			reel.setClick = reel.currentClick;
			reel.PWM = PWM_NEU;
			stepper.flag = 0;
			stepper.isEnabled = FALSE;
			stepper.setPos = stepper.position;
		}//if allStopFlag

	}//main while loop
}// main()

int input_handler (char *instring, char *outstring){
	int retval=0;
	switch (instring[0]){
	case 'R':
		if (instring[1]=='D'){				// Set the reel depth in # of clicks
			reel.setClick=str2num(instring+2,3);
			reel.flag = 1;
			reel.timeout1 = 0;
			reel.timeout2 = 0;
			interruptCode = 0;
			retval=0;
			allStopFlag=0;
		}
		break;
	case 'M':
		if (instring[1]=='D'){				// Set the motor up PWM signal
			reel.PWM_Down=str2num(instring+2,3);
		}
		if (instring[1]=='U'){				// Set the motor down PWM signal
			reel.PWM_Up=str2num(instring+2,3);
		}
		if (instring[1]=='E'){				// Return the motor down PWM signal
			num2str(reel.PWM_Down,outstring,3);
			retval=3;
		}
		if (instring[1]=='V'){				// Return the motor up PWM signal
			num2str(reel.PWM_Up,outstring,3);
			retval=3;
		}
		break;
	case 'C':
		if (instring[1]=='D'){				// Get clicks depth of reel
			num2str(reel.currentClick,outstring,3);
			retval=3;
		}
		if (instring[1]=='C'){				// Manually set current click
			reel.currentClick=str2num(instring+2,3);
		}
		break;
	case 'P':
		if (instring[1]=='U'){					// Pull Up Reel
			reel.setClick = -6;
			reel.flag = 1;
			reel.timeout1 = 0;
			reel.timeout2 = 0;
			interruptCode = 0;
			retval=0;
			allStopFlag=0;
		}
		break;

	case 'L': // Linear Stage Control -----------------------------------------------
		switch(instring[1]){
		case 'H':								// Send stepper motor home
			stepper.flag = 1;
			findHome();
			break;
		case 'P':								// Go To step
			stepper.setPos = str2num(instring+2,3);
			stepper.flag = 1;
			allStopFlag=0;
			break;
		case 'F':								// Move stepper motor forward one position
			stepper.flag = 1;
			stepper.setPos = stepper.position + 1;
			allStopFlag=0;
			break;
		case 'B':								// Move stepper motor backward one position
			stepper.flag = 1;
			stepper.setPos = stepper.position - 1;
			allStopFlag=0;
			break;
		case 'C':								// Return Current stepper position
			num2str(stepper.position,outstring,3);
			retval=3;
			break;
		case 'S':								// Return stepper set position
			num2str(stepper.setPos,outstring,3);
			retval=3;
			break;
		default:
			outstring[0]= instring[0];
			outstring[1]= instring[1];
			outstring[2]= instring[2];
			retval=3;
			break;
		}
		break; // end of Linear stage controls
		case 'W':								// Return Current wrap of the reel
			num2str(reel.currentWrap,outstring,3);
			retval=3;
			break;
		case 'S':								// All Stop
			allStopFlag=1;
			all_stop_fun();
			retval=0;
			break;
		case 'I':								// Comms check for web interface
			outstring[0]= 'O';
			outstring[1]= 'k';
			retval=2;
			break;
		case 'Q':								// Query status
			outstring[0]=(0x30+allStopFlag);
			outstring[1]=(0x30+interruptCode);
			outstring[2]=(0x30+statusCode);
			retval=3;
			break;
		default:								// Default: return command bytes received
			outstring[0]= instring[0];
			outstring[1]= instring[1];
			outstring[2]= instring[2];
			retval=3;
			break;
	}

	return retval;

}

int str2num(char *instring,int n){
	int k,outval=0;
	for (k=0;k<n;k++){
		outval*=10;
		outval+=(instring[k]-0x30);
	}
	return outval;
}

void num2str(int inval,char *outstring,int n){
	int k, neg = 0, divider=1,tval=0,cval;
	if (inval<0){
		inval*=-1;
		neg = 1;
	}
	for (k=0;k<(n);k++){
		divider*=10;
	}
	tval=inval;
	for (k=0;k<n;k++){
		divider/=10;
		cval=tval/divider;
		outstring[k]=cval+0x30;
		tval-=(cval*divider);
	}
	if (neg)
		outstring[0]='-';

}

void all_stop_fun(void){

	// turn off all pwm channels/motors
	TA1CCR2 = 0;
	reel.flag = 0;
	reel.direction = 0;
	stepper.flag = 0;
	stepper.isEnabled = FALSE;
	reel.flag = 0;
	reel.direction = 0;
	reel.setClick = reel.currentClick;
	reel.PWM = PWM_NEU;
	stepper.flag = 0;
	stepper.isEnabled = FALSE;
	stepper.setPos = stepper.position;

	statusCode = 4;
}


