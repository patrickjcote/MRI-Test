#include <msp430.h>
#include "i2c.h"
#include "float.h"
/*
 * main.c
 *
 * This code is an I2C master specifically for the 2016 NRMC team.  This device specifically controls the feedback control of a gyro and acclerometer.
 * You can auto level the platform of that is holding the lidar on the robot.  This also returns values from the gyro and the accelerometer.
 *
 * Pinout
 * Pin 1.0 I2c Master SDA       Configured to interface to the HMC Magnetometer
 * Pin 1.3 I2C Masster SCL
 * Pin 2.0 PWM out for y axis control
 *
 * Sending a # followed by the address of the device returns 13 values.  This consists of the following
 * 0-number of bytes being returned
 * 1-MSB of x axis accelerometer data
 * 2-LSB of x axis accelerometer data
 * 3-MSB of y axis accelerometer data
 * 4-LSB of y axis accelerometer data
 * 5-MSB of z axis accelerometer data
 * 6-LSB of z axis accelerometer data
\ *
 * You can also activate the servos to auto level the platform by executing a * followed by the address and then a 5 digit number.  The five digit number sequence can only be a value of 0,1,2.
 *
 * mode 0:  no auto leveling occuring.
 * Mode 1:  auto level indefinately.
 * Mode 2:  Auto level until it is stable and zerod then stop

 *
 */

#define ADXL_ADDR 0x3A		//Accelerometer Address
#define PWM_MAX	3500		// PWM high limit
#define PWM_MIN  2500		// PWM low limit
#define PWM_NEU  3000		// PWM Neutral limit
#define ANGLE_DIF 50

unsigned char TXData[64],RXData[64];		//Buffers for the Slave of ths device
volatile int TXData_ptr=0,RXData_ptr=0,i2crxflag=0;		//Pointers and flags for the slave device
volatile int sampstate=0,i2cmode=0;  //Sampstate can be 0-idle 1-pumping forward 2-pumping reverse 3-moving sampler
int conv_char_hex(char *,int );
void init_ADXL(void);
void read_ADXL(int *);
int autoLevel(int, int *);


int main(void) {
	int accelvec[3],k;
	float pwmfloatx=0,pwmfloaty=0;
	int pwmcount=0,autolevel=0;
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);

	//Start pwmoutput

	P2DIR |= BIT2; // P2.2 Auto-Level PWM
	P2SEL |= BIT2 ;
	TA1CCR0 = 40000;
	TA1CCR1 = 0;
	TA1CCTL1 = OUTMOD_7;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;



	//Initialize I2c Master code
	i2c_init();
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);


	__delay_cycles(50000);
	init_ADXL();
	__delay_cycles(50000);
	//	init_ADXL();
	//	__delay_cycles(50000);
	//	init_ADXL();
	__bis_SR_register(GIE);  // Enable interrupts (yes this is needed)
	__delay_cycles(50000);
	while(1){
		__delay_cycles(50000);


		read_ADXL(accelvec);

		// Load data to return if asked for via I2C Slave
		TXData[0]=13;
		TXData[2]=(accelvec[0]&0xFF0);
		TXData[1]=((accelvec[0]>>8)&0xFF);
		TXData[4]=(accelvec[1]&0xFF0);
		TXData[3]=((accelvec[1]>>8)&0xFF);
		TXData[6]=(accelvec[2]&0xFF0);
		TXData[5]=((accelvec[2]>>8)&0xFF);
		__delay_cycles(50000);

		autoLevel(1, &accelvec);

	}
}

int autoLevel(int setAngle, int *accelvec){
			if(setAngle == 0){
				TA1CCR1 = 0;
				return 0;
			}
			//code to auto_level
			if(accelvec[0] > (ANGLE_DIF + setAngle))
				TA1CCR1=PWM_MIN;
			else if(accelvec[0] < (-ANGLE_DIF + setAngle))
				TA1CCR1=PWM_MAX;
			else
				TA1CCR1=PWM_NEU;
			return 1;
}//autoLevel




// USCI_B0 Data ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
	if (i2cmode){
		UCB0TXBUF = TXData[TXData_ptr];                       // TX data
		TXData_ptr++;
	}
	else{
		RXData[RXData_ptr]=UCB0RXBUF;
		RXData_ptr++;
		i2crxflag++;
	}

	//  __bic_SR_register_on_exit(CPUOFF);        // Exit LPM0
}

// USCI_B0 State ISR
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
	//	volatile int temp;
	i2cmode=0;

	if(IFG2 & UCB0TXIFG){							// detect beginning of i2c in slave-master mode
		i2cmode=1;
		if (UCB0STAT&UCSTTIFG){
			UCB0STAT &= ~(UCSTPIFG + UCSTTIFG);       // Clear interrupt flags
			TXData_ptr=0;
		}	// Increme
	}
	if(IFG2&UCB0RXIFG){								// detect beginning of i2c in master-slave mode
		i2cmode=0;
		if (UCB0STAT&UCSTTIFG){
			UCB0STAT &= ~(UCSTPIFG + UCSTTIFG);       // Clear interrupt flags
			RXData_ptr=0;
		}	// Increment data
	}

}



int conv_char_hex(char *in_str,int num){
	volatile int k,tempc=0;
	for (k=0;k<num;k++){
		tempc<<=4;
		if (in_str[k]>0x60)
			tempc+=in_str[k]-87;
		else if (in_str[k]>0x40)
			tempc+=in_str[k]-55;
		else
			tempc+=in_str[k]-0x30;

	}
	return tempc;
}


void init_ADXL(void){
	char i2cbuf[12];
	i2cbuf[0]=ADXL_ADDR;
	i2cbuf[1]=0x2D;		//Power control address
	i2cbuf[2]=0x08;		//Turn on the power control
	i2c_rx_bb(i2cbuf,3,0);		//Transmit the power on sequence
	i2cbuf[0]=ADXL_ADDR;
	i2cbuf[1]=0x2C;
	//	i2cbuf[2]=0x00;
	//	i2c_rx_bb(i2cbuf,3,0);
	////
	//	i2cbuf[1]=0x31;		//Resolution address Data Format
	//	i2cbuf[2]=0x88;		//Full range resolution set
	//	i2c_rx_bb(i2cbuf,3,0);		//Transmit the resolution data
	//	i2cbuf[1]=0x30;
	//	i2cbuf[2]=-10;
	//	i2cbuf[3]=-10;
	//	i2cbuf[4]=20;
	//	i2c_rx_bb(i2cbuf,5,0);		//Transmit the resolution data
}

void read_ADXL(int *accel_vec){
	char i2cbuf[12];
	int n,k;
	i2cbuf[0]=ADXL_ADDR;
	i2cbuf[1]=0x32+0x80;
	i2c_rx_bb(i2cbuf,2,0);


	i2cbuf[0]=(ADXL_ADDR+1);

	i2c_rx_bb(i2cbuf,1,6);
	accel_vec[0]=(i2cbuf[1]+((i2cbuf[2]<<8)));
	accel_vec[1]=(i2cbuf[3]+((i2cbuf[4]<<8)));
	accel_vec[2]=(i2cbuf[5]+((i2cbuf[6]<<8)));
	//	accel_vec[0]+=1024;
	//	accel_vec[1]+=1024;
	//	accel_vec[2]+=1024;
	//	for (n=0;n<3;n++){
	//		conv_hex_dec(accel_vec[n]);
	//		for (k=0;k<6;k++)
	//			tx_data_str[k]=dec_str[k];
	//		uart_write_string(0,6);
	//	}
}
