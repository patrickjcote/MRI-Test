#include <msp430.h>
#include "serial_handler.h"
#include "Sampler.h"
/*
 * main.c for sampler
 */

/* Functions' Prototypes*/
int str2num(char *,int );					//converts strings to numbers
int input_handler (char *, char *);			//handles user inputs
void num2str(int ,char *,int );				//converts numbers to strings
void all_stop_fun(void);					//stops arm and pump

/*Variables Initialization:
 *pump_vol: pumping volume(in milliliters),	pump_dir: pumping direction(forward/reverse)
 *new-bottle: value entered by user(1-24),	cur_bottle: bottle where arm is placed
 *k: delay multiplier,	ARM_TO: arm timeout(eliminate false triggering of current sensor)
 *status: status of sampler(0:Ready, 1:pumping forward, 2: pumping reverse, 3:arm moving, 4: arm resetting, 5: ALL_STOP_FLAG triggered)
 *Flags: to activate desired operations and eliminate interference between functions
 *		arm_flag: activates arm function to move arm to desired bottle
 *		arm_reset_flag: resets arm position to bottle #1
 *		pump_flag: activates pump function to fill/drain desired volume
 *		ALL_STOP_FLAG: allows stopping of all operations by user
 */
volatile int pump_vol, pump_dir, pump_flag=0, ALL_STOP_FLAG=0, status=0;
volatile int new_bottle, cur_bottle,arm_flag=0, arm_reset_flag=1, k;
unsigned int ARM_TO;

/*Main Function*/
int main(void) {
	WDTCTL = WDTPW | WDTHOLD;				//Stop watchdog timer
	volatile char identify[]="Sampler";		//character variable with device ID
	volatile int n, ok2send=0;				//local variables - n:num of char in string,	ok2send:checks if string was received

	__delay_cycles(50000);					//delay
	i2c_slave_init(0x52);  					//set slave address to 0x52
	uart_init(4);   						//set uart baud rate to 9600

	BCSCTL1 = CALBC1_16MHZ;                 //set Clock Speed to 16MHz
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)						//delay multiplier of 200
		__delay_cycles(50000);				//delay

	ADC_init();								//initialize ADC peripheral
	sampler_init();							//initialize sampler: both arm and pump
	status = 4;								//status: arm resetting
	ARM_TO = 30;
	arm_reset();							//call arm reset function


	__bis_SR_register(GIE);
	i2cTXData[0]=3;									//Dummy values first input to i2cTXData
	i2cTXData[1]=10;
	i2cTXData[2]=12;

	while(1){										//infinite loop
		if (eos_flag){								//check if data received via UART (UART flag)

			eos_flag=0;								//turn UART flag off(=0)
			if (rx_data_str[0]=='I'){				//if it's the identifying character return the device ID
				for (n=0;n<9;n++)
					tx_data_str[n]=identify[n];
				uart_write_string(0,9);
			}
			else{									//if it's another character call the input handler function
				ok2send=input_handler(rx_data_str,tx_data_str);	//send received data to input_handler and receive output data
				if (ok2send)						//if a value is assigned display to user
					uart_write_string(0,ok2send);

			}
			rx_flag=0;								//set UART receive flag off(=0)
		}
		if (i2crxflag){								//check if data received via the I2C slave
			__delay_cycles(50000);					//delay
			ok2send=input_handler(i2cRXData+1,i2cTXData);		//send received data to input_handler and receive output data
			i2crxflag=0;							//set I2C receive flag off(=0)

		}
		if (!ALL_STOP_FLAG){						//check for ALL STOP FLAG (if 0)
			if(pump_flag){							//if pump flag is activated(=1)
				pump_water(pump_dir,pump_vol);		//send pump direction and pump volume to pumping function
			}
			if(arm_flag){							//if arm flag is activated(=1)
				arm_position(new_bottle);			//send desired bottle number to arm function
			}
			if(arm_reset_flag){						//if arm reset flag is activated(=1)
				arm_reset();						//activate arm resetting function
			}
		}
		if (ALL_STOP_FLAG){							//check for ALL STOP FLAG (if 1)
			P1OUT &=~ PUMP;							//set output to pump low (turn off pump)
			P2OUT |= ARMF + ARMR;					//set outputs to arm high (turn off arm)
		}



	}
}

/*Input Handler Function
 * Receives a command string and returns a specified feedback or status
 */
int input_handler (char *instring, char *outstring){
	int retval=0;
	switch (instring[0]){
	case 'B':									//B: bottle
		if (instring[1]=='P'){						//P: position - assign new bottle position
			new_bottle=str2num(instring+2,3);		//new bottle = three digit number (001-024) after the two characters (e.g. BP011)
			arm_flag=1;								//activate arm flag
			retval=0;								//no returned value
			pump_flag=0;							//turn off pump flag
			arm_reset_flag=0;						//turn off arm reset flag
			ALL_STOP_FLAG=0;						//turn off ALL STOP FLAG
			status = 3;								//status=3 - arm moving
			ARM_TO = 30;							//set arm timeout to 75
		}
		if (instring[1]=='N'){						//N: number - get number of current bottle
			num2str(cur_bottle,outstring,3);		//convert current bottle number to string and display to user
			retval=3;								//return value has 3 characters (digits) e.g. 007
		}
		if (instring[1]=='R'){						//R: reset - reset arm position to bottle 1
			arm_reset_flag=1;						//activate arm reset flag
			retval=0;								//no returned value
			arm_flag=0;								//turn off arm flag
			pump_flag=0;							//turn off pump flag
			ALL_STOP_FLAG=0;						//turn off ALL STOP FLAG
			status = 4;								//status=4 - arm resetting
			ARM_TO = 30;							//set arm timeout to 65000
		}
		break;
	case 'P':									//P: pump
		if (instring[1]=='F'){						//F: forward - fill up bottle
			pump_vol=str2num(instring+2,3);			//sample size = three digit number (001-800)ml after the two characters (e.g. PF300)
			pump_flag=1;							//activate pump flag
			pump_dir=1;								//set pump direction to 1 (forward)
			retval=0;								//no returned value
			arm_flag=0;								//turn off arm flag
			arm_reset_flag=0;						//turn off arm reset flag
			ALL_STOP_FLAG=0;						//turn off ALL STOP FLAG
			status = 1;								//status=1 - pump forward (get sample)
		}
		if (instring[1]=='R'){						//R: reverse - drain sampler hose
			pump_vol=str2num(instring+2,3);			//draining volume = three digit number (001-800)ml after the two characters (e.g. PR300)
			pump_flag=1;							//activate pump flag
			pump_dir=0;								//set pump direction to 0 (reverse)
			retval=0;								//no returned value
			arm_flag=0;								//turn off arm flag
			arm_reset_flag=0;						//turn off arm reset flag
			ALL_STOP_FLAG=0;						//turn off ALL STOP FLAG
			status = 2;								//status=2 - pump reverse (drain sampler hose)
		}
		break;
	case 'S':									//S: stop
		ALL_STOP_FLAG=1;							//activate ALL STOP FLAG
		pump_flag=0;								//turn off pump flag
		arm_flag=0;									//turn off arm flag
		arm_reset_flag=0;							//turn off arm reset flag
		status = 5;									//status=5 - ALL STOP FLAG activated
		all_stop_fun();								//call function to stop all sampler operations
		retval=0;									//no returned value
		break;
	case 'Q':									//Q: query - get status of the sampler
		outstring[0]='S';							//display 'S'
		outstring[1]=':';							//display ':'
		num2str(status,outstring+2,1);				//display status (e.g. S:0 - sampler is ready)
		retval=3;									//return value has 3 characters
		break;
	case 'I':									//I: info - get connectivity status of the sampler
		outstring[0]='O';							//display 'O'
		outstring[1]='k';							//display 'k'
		retval=2;									//return value has 2 characters
		break;
	default:									//If incorrect command is entered display the first 3 characters of the command
		outstring[0]= instring[0];
		outstring[1]= instring[1];
		outstring[2]= instring[2];
		retval=3;
		break;
	}

	return retval;

}

/*Function to convert strings to numbers*/
int str2num(char *instring,int n){
	int k,outval=0;
	for (k=0;k<n;k++){
		outval*=10;
		outval+=(instring[k]-0x30);
	}
	return outval;
}

/*Function to convert numbers to strings*/
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

/*Function to stop ALL sampler operations*/
void all_stop_fun(void){
	P1OUT &=~ PUMP;							//set output to pump low (turn off pump)
	P2OUT |= ARMF + ARMR;					//set outputs to arm high (turn off arm)
}


