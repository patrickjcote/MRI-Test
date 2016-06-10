#include <msp430.h>
#include "serial_handler.h"
/*
 * main.c
 */

int str2num(char *,int );
int input_handler (char *, char *);
void num2str(int ,char *,int );

int set_reel_depth=0, cur_reel_depth=-45,reel_flag=0,ALL_STOP_FLAG=1;


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	volatile char identify[]="Identify_your_device";
	volatile int n, ok2send=0;
	__delay_cycles(50000);
	i2c_slave_init(0x48);  //Set slave address to 0x48
	uart_init(4);   // set uart baud rate to 9600


	__bis_SR_register(GIE);
	i2cTXData[0]=3;			//Dummy values first input to i2cTXData
	i2cTXData[1]=10;
	i2cTXData[2]=12;
	while(1){
		if (eos_flag){					// if data received from the UART

			eos_flag=0;
			if (rx_data_str[0]=='I'){	// If it is the identifying character return the device ID
				for (n=0;n<9;n++)
					tx_data_str[n]=identify[n];
				uart_write_string(0,9);
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
			i2crxflag=0;

		}
		if (!ALL_STOP_FLAG){
			//check flags of all devices
		}



	}
}

int input_handler (char *instring, char *outstring){
	int retval=0;
	switch (instring[0]){
	case 'R':
		if (instring[1]=='D'){				// Set values for the depth of clicks the reel will go to
			set_reel_depth=str2num(instring+2,3);
			reel_flag=1;
			retval=0;
		}
		break;
	case 'C':
		if (instring[1]=='D'){				// Set values for the depth of clicks the reel will go to
			num2str(cur_reel_depth,outstring,3);
			retval=3;
		}
		break;
	case 'P':
		if (instring[1]=='U'){				// Set values for the depth of clicks the reel will go to
			set_reel_depth=-1;
			reel_flag=1;
			retval=0;
		}
		break;
	case 'S':
		ALL_STOP_FLAG=1;
		all_stop_fun();
		retval=0;
		break;
	case 'Q':
		outstring[0]=(0x30+1-ALL_STOP_FLAG);
		retval=1;
		break;
	default:
		outstring[0]='N';
		outstring[1]='o';
		retval=2;
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
	// turn off all pwm channels and all motors
}


