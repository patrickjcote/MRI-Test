/*
 * i2c.c
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */
#include "msp430.h"
#define tide 365
//volatile char i2cbuf[16];

void i2c_init(void){
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	P1DIR |= (BIT6+BIT7);
	P1OUT |= (BIT6+BIT7);
}


void wait_burn(int cycles){
	volatile int	k;
	for(k=cycles;k>0;k--){
		__delay_cycles(5000);
	}

}

int i2c_rx_bb(char *i2cbuf,int txnum, int rxnum){
	volatile unsigned int i,k, temp;
	//		wait_burn(100);
	P1OUT|=BIT6;				//CLK High
	P1DIR |=BIT7;				//Data Output

	P1OUT &=~BIT7;				//Data low
	__delay_cycles(tide);
	P1OUT&=~BIT6;				//Clk low
	__delay_cycles(2);
	for (i=0;i<txnum;i++){
		temp=i2cbuf[i];
		for (k=0;k<8;k++){
			__delay_cycles(tide);
			if ((temp&0x80)==0x80){
				P1OUT |=BIT7;
			}
			else{
				P1OUT&=~BIT7;
			}
			P1OUT|=BIT6;		//CLK High
			__delay_cycles(tide);
			temp<<=1;
			P1OUT&=~BIT6;		//CLK Low

		}
		P1DIR &=~BIT7;				//Set input on data for ACK

		__delay_cycles(tide);
		P1OUT|=BIT6;				//CLK high
		__delay_cycles(tide);
		if (P1IN&BIT7){				//  Acknowledge missed, stop condition ensues
			P1DIR|=BIT7;			// Set data as output
			P1OUT&=~BIT7;			// Data low
			P1OUT&=~BIT6;			//CLK Low
			__delay_cycles(tide);
			P1OUT|=BIT6;			//CLK High
			__delay_cycles(tide);			//Stop Condition
			P1OUT |=BIT7;			//Data High
			return 1;
		}
		P1OUT &=~BIT6;				//CLK Low
		P1DIR|=BIT7;				//Set direction to output for data
		__delay_cycles(tide);
		P1OUT&=~BIT7;
	}

	P1DIR&=~BIT7;
	P1OUT&=~BIT7;
	if (rxnum==-1)
		rxnum=100;

	for (i=(txnum); i<(txnum+rxnum); i++){
		temp=0x00;
		P1DIR&=~BIT7;
		for (k=0; k<8; k++){
			temp<<=1;
			__delay_cycles(tide);

			P1OUT|=BIT6;		//CLK High
			__delay_cycles(tide);
			if ((P1IN&BIT7)==BIT7){
				temp|=0x01;
			}
			else{
				temp&=~0x01;
			}
			//				__delay_cycles(7);
			P1OUT&=~BIT6;		//CLK Low

			P1OUT |=BIT7;
		}

		P1DIR |=BIT7;				//Set output for Master ACK
		if (i==(txnum+rxnum-1)){		// make NACK
			P1OUT |=BIT7;				//Data High for NACK
		}
		else{					//Master send ACK
			P1OUT &=~BIT7;				//Data Low
			__delay_cycles(tide);

		}
		if (i==txnum){
			if (rxnum==100)
				rxnum=temp;
		}

		i2cbuf[i]=temp;


		__delay_cycles(tide);
		P1OUT|=BIT6;				//CLK high
		__delay_cycles(tide*3);



		P1OUT &=~BIT6;				//CLK Low

		__delay_cycles(tide);
		P1OUT &=~BIT7;				//Data Low
		P1DIR&=~BIT7;



	}
	P1DIR |=BIT7;				//Set output
	P1OUT &=~BIT7;				//Data low
	__delay_cycles(tide);
	P1OUT|=BIT6;				//CLK high
	__delay_cycles(tide);
	P1OUT|=BIT7;				// Data high: Stop bit
	return 0;


}






