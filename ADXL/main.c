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
 * Pin 1.1 I2C Masster SCL
 * Pin 1.2 PWM out for y axis control
 * Pin 1.6 I2C Slave SCL  (Slave to master controller)
 * Pin 1.7 I2C Slave SDA
 * Pin 2.1 PWM out for x axis control
 *
 * Sending a # followed by the address of the device returns 13 values.  This consists of the following
 * 0-number of bytes being returned
 * 1-MSB of x axis accelerometer data
 * 2-LSB of x axis accelerometer data
 * 3-MSB of y axis accelerometer data
 * 4-LSB of y axis accelerometer data
 * 5-MSB of z axis accelerometer data
 * 6-LSB of z axis accelerometer data
 * 7-MSB of x axis gyro data
 * 8-LSB of x axis gyro data
 * 9-MSB of x axis gyro data
 * 10-LSB of x axis gyro data
 * 11-MSB of x axis gyro data
 * 12-LSB of x axis gyro data
 *
 * The accelerometer value is unsigned but has an offset of 1024 since it is a 10 bit value.  In other words a value of 1024 means no acceleration being experienced
 *   A full scale value indicates 2Gs worth of acceleration
 * The gyro value is unsigned but has an offset of 0x7FFF or 32767.  In other words the value 32767 means no moment of inertia being measured
 * A full scale value of 0xffff means it is experiencing 250 dps (degrees per second)
 *
 * You can also activate the servos to auto level the platform by executing a * followed by the address and then a 5 digit number.  The five digit number sequence can only be a value of 0,1,2.
 *
 * mode 0:  no auto leveling occuring.
 * Mode 1:  auto level indefinately.
 * Mode 2:  Auto level until it is stable and zerod then stop

 *
 */

#define addr_mpu 0xD2	// address of gyro/accel
#define ADXL_ADDR 0x3A		//Accelerometer Address
#define PWMMAX	4500		// PWM high limit
#define PWMMIN  1500		// PWM low limit
#define PWMNEU  3000		// PWM Neutral limit
#define kcontroller 1		//controller gain to set auto-level
#define kcontrollerd 8

unsigned char TXData[64],RXData[64];		//Buffers for the Slave of ths device
volatile int TXData_ptr=0,RXData_ptr=0,i2crxflag=0;		//Pointers and flags for the slave device
volatile int sampstate=0,i2cmode=0;  //Sampstate can be 0-idle 1-pumping forward 2-pumping reverse 3-moving sampler
volatile unsigned int pwmread[]={0,0,0,0,0,0,0,0},pwmval[8],pwmflag=0,pwmtimeout=0;
int conv_char_hex(char *,int );
void init_L3G4200D();
void read_L3G4200D( int *);
void init_ADXL(void);
void read_ADXL(int *);


int main(void) {
	int accelvec[3],k,PWMVAL[]={PWMNEU,PWMNEU};
	unsigned int gyrovec[3];
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
	P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
	P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
	P1DIR &=~(BIT0+BIT1);

	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
	UCB0CTL0 = UCMODE_3 + UCSYNC;             // I2C Slave, synchronous mode
	UCB0I2COA = 0x25;            		// Own Address.  Change this when you specialze this code
	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
	UCB0I2CIE |= UCSTTIE;                     // Enable STT interrupt
	IE2 |= (UCB0TXIE+UCB0RXIE);                          // Enable TX interrupt
	//  The code below is an ADC code specific to reading the internal temperature

	//Start pwmoutput

	P2DIR |= BIT1; // P2.1 y P2.4 como salida
	P2SEL |= BIT1 ; // Asociado al Timer_A1

	TA1CCR0 = 40000; // Cargamos el periodo PWM
	TA1CCR1 = 0; // PWM duty cycle, 10% CCR1/(CCR0+1) * 100
	TA1CCTL1 = OUTMOD_7; //Modo7 reset/set
	TA1CTL = TASSEL_2 + MC_1+ID_3; // Timer SMCLK Modo UP
	P1DIR |= BIT2; // P2.1 y P2.4 como salida
	P1SEL |= BIT2 ; // Asociado al Timer_A1
	TA0CCR0 = 40000; // Cargamos el periodo PWM
	TA0CCR1 = 0; // PWM duty cycle, 10% CCR1/(CCR0+1) * 100
	TA0CCTL1 = OUTMOD_7; //Modo7 reset/set
	TA0CTL = TASSEL_2 + MC_1+ID_3; // Timer SMCLK Modo UP


	//Initialize I2c Master code
	i2c_init();
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	//Initialize magnetometer
	init_L3G4200D();
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

		if (i2crxflag){     // Check to see if a value has been recieved via I2C Slave
			//			__delay_cycles(50000);
			i2crxflag=0;
			autolevel=(RXData[1]);
		}
		//Read the HMC5883L.  Load value into the feedback buffer
		read_L3G4200D(gyrovec);// Read channel 0 on multiplexer of magnetometer
		read_ADXL(accelvec);

		// Load data to return if asked for via I2C Slave
		TXData[0]=13;
		TXData[2]=(accelvec[0]&0xFF0);
		TXData[1]=((accelvec[0]>>8)&0xFF);
		TXData[4]=(accelvec[1]&0xFF0);
		TXData[3]=((accelvec[1]>>8)&0xFF);
		TXData[6]=(accelvec[2]&0xFF0);
		TXData[5]=((accelvec[2]>>8)&0xFF);
		TXData[7]=(gyrovec[0]&0xFF);
		TXData[8]=((gyrovec[0]>>8)&0xFF);
		TXData[10]=(gyrovec[1]&0xFF);
		TXData[9]=((gyrovec[1]>>8)&0xFF);
		TXData[12]=(gyrovec[2]&0xFF);
		TXData[11]=((gyrovec[2]>>8)&0xFF);
		__delay_cycles(50000);

		if (autolevel){
			//code to auto_level
			pwmfloatx+=(1*(accelvec[1]-900))*kcontroller/kcontrollerd;
			if (pwmfloatx>(PWMMAX-PWMNEU))
				pwmfloatx=(PWMMAX-PWMNEU);
			if (pwmfloatx<(PWMMIN-PWMNEU))
				pwmfloatx=(PWMMIN-PWMNEU);
			TA1CCR1=pwmfloatx+PWMNEU;
			pwmfloaty+=(-1*(accelvec[0]-944))*kcontroller/kcontrollerd;
			if (pwmfloaty>(PWMMAX-PWMNEU))
				pwmfloaty=(PWMMAX-PWMNEU);
			if (pwmfloaty<(PWMMIN-PWMNEU))
				pwmfloaty=(PWMMIN-PWMNEU);
			TA0CCR1=pwmfloaty+PWMNEU;
			if (autolevel==2)
				if ((accelvec[0]<(900+10))&&(accelvec[0]>(900-10))){
					if ((accelvec[1]<(944+10))&&(accelvec[1]>(94-10))){
						pwmcount++;
					}
				}
			if (pwmcount>10){
				autolevel=0;
				pwmcount=0;

			}


		}


	}
}




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







void init_L3G4200D(){
	char i2cbuf[8];
	i2cbuf[0]=addr_mpu;
	i2cbuf[1]=0x20;
	i2cbuf[2]=0xff;
	//	i2cbuf[3]=0xff;
	//	i2cbuf[4]=0xff;
	//	i2cbuf[5]=0xff;

	i2c_rx_bb(i2cbuf,3,0);
	//	i2cbuf[0]=addr_mpu;
	//	i2cbuf[1]=0x1C;
	//	i2cbuf[2]=0x00;
	//	i2c_rx_bb(i2cbuf,3,0);
}



void read_L3G4200D( int *gyrovec){
	char i2cbuf[12];
	int k;
	i2cbuf[0]=addr_mpu;
	i2cbuf[1]=(0x28+0x80);  // the 0x80 for auto increment
	i2c_rx_bb(i2cbuf,2,0);
	__delay_cycles(5000);
	i2cbuf[0]=addr_mpu+1;
	i2c_rx_bb(i2cbuf,1,6);
	for (k=0;k<3;k++){
		gyrovec[k]=(i2cbuf[k*2+2]<<8)+i2cbuf[k*2+1];
		gyrovec[k]+=0x7fff;
	}
	//	for (k=0;k<3;k++){
	//		i2cbuf[0]=addr_mpu;
	//		i2cbuf[1]=0x3B+k*2;
	//		i2c_rx_bb(i2cbuf,2,0);
	//		__delay_cycles(5000);
	//		i2cbuf[0]=addr_mpu+1;
	//		i2c_rx_bb(i2cbuf,1,1);
	//		__delay_cycles(5000);
	//		accelvec[k]=(i2cbuf[1]<<8);
	//		i2cbuf[0]=addr_mpu;
	//		i2cbuf[1]=0x3B+k*2+1;
	//		i2c_rx_bb(i2cbuf,2,0);
	//		__delay_cycles(5000);
	//		i2cbuf[0]=addr_mpu+1;
	//		accelvec[k]+=i2cbuf[1];
	//	}
	//	for (k=0;k<3;k++){
	//		i2cbuf[0]=addr_mpu;
	//		i2cbuf[1]=0x43+k*2;
	//		i2c_rx_bb(i2cbuf,2,0);
	//		__delay_cycles(5000);
	//		i2cbuf[0]=addr_mpu+1;
	//		i2c_rx_bb(i2cbuf,1,1);
	//		__delay_cycles(5000);
	//		gyrovec[k]=(i2cbuf[1]<<8);
	//		i2cbuf[0]=addr_mpu;
	//		i2cbuf[1]=0x43+k*2+1;
	//		i2c_rx_bb(i2cbuf,2,0);
	//		__delay_cycles(5000);
	//		i2cbuf[0]=addr_mpu+1;
	//		gyrovec[k]+=i2cbuf[1];
	//	}


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


// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
	volatile int k,bit_slider=1,statechange=0,n;
	bit_slider=1;
	//	pwm_state=P2IN;
	for (k=0;k<8;k++){
		//		statechange=(bit_slider&(pwm_state^P2IN));
		if (P2IFG&bit_slider){

			if (P2IN&bit_slider){
				if (TA0R>(0xFFFF-4000)){
					for(n=0;n<8;n++)
						pwmread[n]-=TA0R;
					TA0R=0;
				}
				pwmread[k]=TA0R;
				P2IES |= bit_slider;
			}
			else{
				pwmval[k]=TA0R-pwmread[k];
				pwmflag|=bit_slider;
				P2IES &=~ bit_slider;
			}

			P2IFG &= ~bit_slider;                           // P1.4 IFG cleared

		}
		bit_slider<<=1;
	}
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
	accel_vec[0]+=1024;
	accel_vec[1]+=1024;
	accel_vec[2]+=1024;
	//	for (n=0;n<3;n++){
	//		conv_hex_dec(accel_vec[n]);
	//		for (k=0;k<6;k++)
	//			tx_data_str[k]=dec_str[k];
	//		uart_write_string(0,6);
	//	}
}
