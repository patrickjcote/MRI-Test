/*
 * i2c.c
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */
#include "msp430x22x4.h"
#define tide 35
//volatile char i2cbuf[16];

void i2c_init(void){
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	P2DIR |= 0x18;
	P2OUT |= 0x18;
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
	P2OUT|=0x010;				//CLK High
	P2DIR |=0x08;				//Data Output

	P2OUT &=~0x08;				//Data low
	__delay_cycles(tide);
	P2OUT&=~0x10;				//Clk low
	__delay_cycles(2);
	for (i=0;i<txnum;i++){
		temp=i2cbuf[i];
		for (k=0;k<8;k++){
			__delay_cycles(tide);
			if ((temp&0x80)==0x80){
				P2OUT |=0x08;
			}
			else{
				P2OUT&=~0x08;
			}
			P2OUT|=0x10;		//CLK High
			__delay_cycles(tide);
			temp<<=1;
			P2OUT&=~0x10;		//CLK Low

		}
		P2DIR &=~0x08;				//Set input on data for ACK

		__delay_cycles(tide);
		P2OUT|=0X10;				//CLK high
		__delay_cycles(tide);
		if (P2IN&0x08){				//  Acknowledge missed, stop condition ensues
			P2DIR|=0x08;			// Set data as output
			P2OUT&=~0x08;			// Data low
			P2OUT&=~0x10;			//CLK Low
			__delay_cycles(tide);
			P2OUT|=0x10;			//CLK High
			__delay_cycles(tide);			//Stop Condition
			P2OUT |=0x08;			//Data High
			return 1;
		}
		P2OUT &=~0X10;				//CLK Low
		P2DIR|=0x08;				//Set direction to output for data
		__delay_cycles(tide);
		P2OUT&=~0x08;
	}

	P2DIR&=~0x08;
	P2OUT&=~0x08;
	if (rxnum==-1)
		rxnum=100;

	for (i=(txnum); i<(txnum+rxnum); i++){
		temp=0x00;
		P2DIR&=~0x08;
		for (k=0; k<8; k++){
			temp<<=1;
			__delay_cycles(tide);

			P2OUT|=0x10;		//CLK High
			__delay_cycles(tide);
			if ((P2IN&0x08)==0x08){
				temp|=0x01;
			}
			else{
				temp&=~0x01;
			}
			//				__delay_cycles(7);
			P2OUT&=~0x10;		//CLK Low

			P2OUT |=0x08;
		}

		P2DIR |=0x08;				//Set output for Master ACK
		if (i==(txnum+rxnum-1)){		// make NACK
			P2OUT |=0x08;				//Data High for NACK
		}
		else{					//Master send ACK
			P2OUT &=~0x08;				//Data Low
			__delay_cycles(tide);

		}
		if (i==txnum){
			if (rxnum==100)
				rxnum=temp;
		}

		i2cbuf[i]=temp;


		__delay_cycles(tide);
		P2OUT|=0X10;				//CLK high
		__delay_cycles(tide*3);



		P2OUT &=~0X10;				//CLK Low

		__delay_cycles(tide);
		P2OUT &=~0x08;				//Data Low
		P2DIR&=~0x08;



	}
	P2DIR |=0x08;				//Set output
	P2OUT &=~0x08;				//Data low
	__delay_cycles(tide);
	P2OUT|=0X10;				//CLK high
	__delay_cycles(tide);
	P2OUT|=0x08;				// Data high: Stop bit
	return 0;


}






