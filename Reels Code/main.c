#include <msp430.h>
#include "reels.h"
#include "i2c.h"
#include "float.h"

volatile int clicks;
volatile int reelDepth;
unsigned char TXData[64],RXData[64];		//Buffers for the Slave of ths device
volatile int TXData_ptr=0,RXData_ptr=0,i2crxflag=0;		//Pointers and flags for the slave device
volatile int sampstate=0,i2cmode=0;  //Sampstate can be 0-idle 1-pumping forward 2-pumping reverse 3-moving sampler


int main(void) {

	int accelvec[3],k;

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	for (k=0;k<200;k++)
		__delay_cycles(50000);

	//Initialize I2c Master code
	i2c_init();
	__delay_cycles(50000);__delay_cycles(50000);
	__delay_cycles(50000);__delay_cycles(50000);

	init_ADXL();
	__delay_cycles(50000);

	__bis_SR_register(GIE);  // Enable interrupts (yes this is needed)
	__delay_cycles(50000);

	initReel();

	while(1){
		__delay_cycles(50000);

		read_ADXL(accelvec, TXData);

		autoLevel(1, &accelvec);

	}


	return 0;
}//main()


void initReel(){

	//Limit Switch input
	P1DIR &= ~BIT4;				// Limit switch input on 1.4
	P1IE |=  BIT4;				// 1.4 interrupt enabled
	P1IES |= BIT4;				// 1.4 Hi/lo edge
	P1REN |= BIT4;				// Enable Pull Up (P1.4)
	P1IFG &= ~BIT4;				// P1.4 IFG clear
	//Click Count input
	P2DIR &= ~BIT0;				// Click count input on 2.0
	P2IE |=  BIT0;				// P2.0 interrupt enabled
	P2IES |= BIT0;				// P2.0 Hi/lo edge
	P2REN |= BIT0;				// Enable Pull Up (P2.0)
	P2IFG &= ~BIT0;				// P2.0 IFG clear
	//PWM Init
	TA1CCR0 = 40000;
	TA1CCR1 = 0;
	TA1CCR2 = 0;
	TA1CCTL1 = OUTMOD_7;
	TA1CCTL2 = OUTMOD_7;
	TA1CTL = TASSEL_2 + MC_1 + ID_3;
	//PWM Outputs
	P2DIR |= BIT4;				//Motor Control on P2.4
	P2SEL |= BIT4;				//TA1.2 Output on P2.4
	P2DIR |= BIT2;				// P2.2 Auto-Level PWM
	P2SEL |= BIT2;				//TA1.1 Output to  P2.2
	//Init Global Variables
	clicks = 0;
	reelDepth = 0;

	__bis_SR_register(GIE);

}//init_reel()


int pullUpReel(){

	while(reelDepth > 0){
		TA1CCR2 = PWM_MAX;
		//TODO: Autolevel code
	}
	TA1CCR2 = PWM_NEU;

	return 0;
}//pullUpReel()

int autoLevel(int angle, int *accelvec){
	if(setAngle == 0){
		TA1CCR1 = 0;
		return 0;
	}
	//code to auto_level
	if(accelvec[0] > (ANGLE_DIF + angle))
		TA1CCR1=PWM_MIN;
	else if(accelvec[0] < (-ANGLE_DIF + angle))
		TA1CCR1=PWM_MAX;
	else
		TA1CCR1=PWM_NEU;
	return 1;
}//autoLevel

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

void read_ADXL(int *accel_vec, int *TXData){
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
	// Load data to return if asked for via I2C Slave
	TXData[0]=13;
	TXData[2]=(accel_vec[0]&0xFF0);
	TXData[1]=((accel_vec[0]>>8)&0xFF);
	TXData[4]=(accel_vec[1]&0xFF0);
	TXData[3]=((accel_vec[1]>>8)&0xFF);
	TXData[6]=(accel_vec[2]&0xFF0);
	TXData[5]=((accel_vec[2]>>8)&0xFF);
	__delay_cycles(50000);
}

// --------------------------------  Interrupts ----------------------
// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	//Hardware interrupt for limit switch
	clicks = 0;
	P1IFG &= ~BIT4;
}

// Port 2 ISR
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	//Hardware interrupt for click counter
	clicks++;
	P2IFG &= ~BIT0;
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

