#include <msp430.h>
#include "serial_handler.h"
#include "stepper.h"




int str2num(char *,int );
int input_handler (char *, char *);
void num2str(int ,char *,int );

volatile int ALL_STOP_FLAG;
volatile int stepper_pos, step_count, stepper_dir, stepper_en;


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	volatile char identify[]="STEP";
	volatile int n, k, ok2send=0;
	__delay_cycles(50000);
	i2c_slave_init(0x53);  //Set slave address to 0x53
	uart_init(4);   // set uart baud rate to 9600

	BCSCTL1 = CALBC1_16MHZ;                    // Set Clock Speed
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);

	initStepper();
	__delay_cycles(50000);

	ALL_STOP_FLAG = 1;

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
			ok2send=input_handler(i2cRXData+1,i2cTXData);
			i2crxflag=0;

		}
		if (!ALL_STOP_FLAG){

		}
		if (ALL_STOP_FLAG){
			stepper_en = 0;
		}
	}//
}

int input_handler (char *instring, char *outstring){
	int retval=0;
	switch (instring[0]){
	case 'S':
		if(instring[1] == 'F')
			step(FORWARD);
		if(instring[1] == 'B')
			step(BACKWARD);
		retval=0;
		break;
	case 'P':			// Get current Position
		num2str((stepper_pos/STEPS_PER_CLICK),outstring,3);
		retval=3;
		break;
	case 'H':			// Find Home Position
		findHome();
		retval=0;
		break;
	case 'G':			// Go To Step
		goToStep(str2num(instring+1,3)*STEPS_PER_CLICK);
		retval=0;
		break;
	default:
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
