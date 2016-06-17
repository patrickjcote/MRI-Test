#include <msp430.h>
#include "serial_handler.h"

#define PURGE_TIME1 3000
#define PURGE_TIME2 1		//Minutes


int str2num(char *,int );
int input_handler (char *, char *);
void num2str(int ,char *,int );
void all_stop_fun(void);

volatile int purge_flag, valve_dir, ALL_STOP_FLAG;
volatile unsigned int purge_count1, purge_count2;


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	volatile char identify[]="Valve";
	volatile int n, k, ok2send=0;
	__delay_cycles(50000);
	i2c_slave_init(0x50);  //Set slave address to 0x48
	uart_init(4);   // set uart baud rate to 9600

	BCSCTL1 = CALBC1_16MHZ;                    // Set Clock Speed
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);

	//Init Valves Board
	//Purge timer
	TA1CCR0 = 40000;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	TA1CCTL0 = CCIE;
	//LED Indicator and MCP Reset
	P2DIR |= BIT5 + BIT6;
	P2OUT |= BIT5 + BIT6;
	purge_flag = 0;
	purge_count1 = 0;
	purge_count2 = 0;

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
			if(purge_flag){
				P1OUT |= BIT1;
			}

		}
		if (ALL_STOP_FLAG){

		}
	}//
}

int input_handler (char *instring, char *outstring){
	int retval=0;
	switch (instring[0]){
	case 'P':
		purge_flag = 1;
		ALL_STOP_FLAG=0;
		retval = 0;
		break;
	case 'S':
		ALL_STOP_FLAG=0;
		retval = 0;
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

void all_stop_fun(void){
	// turn off all pwm channels and all motors
	TA0CCR1 = 0;
	TA0CCR2 = 0;
	TA1CCR1 = 0;
	TA1CCR2 = 0;
}

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	if(purge_flag){
		P2OUT ^= BIT5;
		purge_count1++;
		if(purge_count1 > PURGE_TIME1)
			purge_count2++;
		if(purge_count2 > PURGE_TIME2){
			purge_flag = 0;
			purge_count1 = 0;
			purge_count2 = 0;
		}
	}
}
