#include <msp430.h>
#include "serial_handler.h"

#define RAMP_STEP_TIME 1 // seconds
#define RAMP_STEP_SIZE 100 // PWM step size

#define FORWARD	4000		// PWM high limit
#define BACKWARD  2000		// PWM low limit
#define STOPPED  3000		// PWM Neutral limit

int str2num(char *,int );
int input_handler (char *, char *);
void num2str(int ,char *,int );
void all_stop_fun(void);

volatile int ramp_flag, ramp_step_flag, pump_flag, pump_speed, valve_dir, ALL_STOP_FLAG;
volatile int set_pump_dir;
volatile unsigned int ramp_timer, ramp_compare, pump_clicks, set_pump_clicks;


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	volatile int n, k, ok2send=0;
	__delay_cycles(50000);


	i2c_slave_init(0x53);
	volatile char identify[]="Valve2";
	uart_init(4);   // set uart baud rate to 9600

	BCSCTL1 = CALBC1_16MHZ;                    // Set Clock Speed
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);


	//LED Indicator and MCP Reset
	P2DIR |= BIT5 + BIT6;
	P2OUT |= BIT5 + BIT6;

	//Ramp Timer
	TA1CCR0 = 0xFFFF;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	TA1CCTL0 = CCIE;

	//Photogate input
	P1DIR &= ~BIT5;				// Photo Gate input on 1.5
	P1IE |=  BIT5;				// 1.5 interrupt enabled
	P1IES |= BIT5;				// 1.5 Hi/lo edge
	P1IFG &= ~BIT5;				// P1.5 IFG clear

	//Valve output
	P2DIR |= BIT0;
	P2OUT &= ~BIT0;

	//Flags/variables init
	pump_flag = 0;
	pump_speed = STOPPED;

	set_pump_dir = STOPPED;
	ramp_timer = 0;
	ramp_compare = 0;
	ramp_step_flag = 0;
	ramp_flag = 0;
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
			if(pump_flag)
				pump(set_pump_dir);
			if(pump_clicks > set_pump_clicks)
				pump_flag = 0;
		}
		if (ALL_STOP_FLAG){
			P2OUT |= BIT5;
			pump_flag=0;
			ramp_flag = 0;
			ramp_timer = 0;
		}
	}//
}

int input_handler (char *instring, char *outstring){
	int retval=0;
	switch (instring[0]){
	case 'R':	//Ramp Pump
	case 'P':	//Manualc pump
		if(instring[0]=='R')
			ramp_flag = 1;
		else
			ramp_flag = 0;
		pump_flag = 1;
		pump_clicks = 0;
		ALL_STOP_FLAG = 0;

		if(instring[1] == 'F')	// Foward
			set_pump_dir = FORWARD;
		if(instring[1] == 'B')	// Backward
			set_pump_dir = FORWARD;
		if(instring[1] == 'S')	// Stop
			set_pump_dir = STOPPED;

		set_pump_clicks=str2num(instring+2,3);

		retval = 0;
		break;
	case 'V':	//Change valve Position
		if(instring[1] == 'C')	// Close
			P2OUT |= BIT0;
		if(instring[1] == 'O')	// Open
			P2OUT &= ~BIT0;
		retval = 0;
		break;
	case 'I':		// Comms check
		outstring[0]= 'O';
		outstring[1]= 'k';
		retval=2;
		break;
	case 'S':		// All Stop
		ALL_STOP_FLAG=1;
		pump_flag=0;
		retval = 0;
		ramp_timer = 0;
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

void pump(int direction){

	if(!ramp_flag){
		pump_speed = direction;
	}
	else if(ramp_flag && ramp_step_flag){
		if(pump_speed < direction){
			pump_speed += RAMP_STEP_SIZE;
			if(pump_speed > direction){
				ramp_flag = 0;
				pump_speed = direction;
			}
		}// if ramping Reverse->Stopped->Forward
		else if(pump_speed > direction){
			pump_speed -= RAMP_STEP_SIZE;
			if(pump_speed < direction){
				ramp_flag = 0;
				pump_speed = direction;
			}
		}// else if ramping Forward->Stopped->Reverse
		ramp_step_flag = 0;
	}// else ramping

}// pump()

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	//Ramp Timer
	if(pump_flag){
		P2OUT ^= BIT5; // Flash status LED
		ramp_compare++;
		if(ramp_compare > 30){
			// (ramp_compare == 30) == 1 second
			ramp_timer++;
			ramp_compare = 0;
		}
		if(ramp_timer > RAMP_STEP_TIME){
			ramp_step_flag=1;
			ramp_timer = 0;
		}
	}


}

// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

	if(P1IFG & BIT5){	// Photo gate edge hit
		pump_clicks++;
		P1IFG &= ~BIT5;
	}
}
