/*
 * i2c.c
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */
#include "msp430.h"
#define tide 200
#define SDA BIT0
#define SCL BIT3
//volatile char i2cbuf[16];

void i2c_init(void){
	BCSCTL1 = CALBC1_16MHZ;                    // Set DCO
	DCOCTL = CALDCO_16MHZ;
	P1DIR |= (SDA+SCL);
	P1OUT |= (SDA+SCL);
	__delay_cycles(50000);
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
	P1OUT|=SCL;				//CLK High
	P1DIR |=SDA;				//Data Output

	P1OUT &=~SDA;				//Data low
	__delay_cycles(tide);
	P1OUT&=~SCL;				//Clk low
	__delay_cycles(2);
	for (i=0;i<txnum;i++){
		temp=i2cbuf[i];
		for (k=0;k<8;k++){
			__delay_cycles(tide);
			if ((temp&0x80)==0x80){
				P1OUT |=SDA;
			}
			else{
				P1OUT&=~SDA;
			}
			P1OUT|=SCL;		//CLK High
			__delay_cycles(tide);
			temp<<=1;
			P1OUT&=~SCL;		//CLK Low

		}
		P1DIR &=~SDA;				//Set input on data for ACK
		__delay_cycles(tide);
		P1OUT|=SCL;				//CLK high
		__delay_cycles(tide);
		if (P1IN&SDA){				//  Acknowledge missed, stop condition ensues
			P1DIR|=SDA;			// Set data as output
			P1OUT&=~SDA;			// Data low
			P1OUT&=~SCL;			//CLK Low
			__delay_cycles(tide);
			P1OUT|=SCL;			//CLK High
			__delay_cycles(tide);			//Stop Condition
			P1OUT |=SDA;			//Data High
			return 1;
		}
		P1OUT &=~SCL;				//CLK Low
		P1DIR|=SDA;				//Set direction to output for data
		__delay_cycles(tide);
		P1OUT&=~SDA;
	}

	P1DIR&=~SDA;
	P1OUT&=~SDA;
	if (rxnum==-1)
		rxnum=100;

	for (i=(txnum); i<(txnum+rxnum); i++){
		temp=0x00;
		P1DIR&=~SDA;
		for (k=0; k<8; k++){
			temp<<=1;
			__delay_cycles(tide);

			P1OUT|=SCL;		//CLK High
			__delay_cycles(tide);
			if ((P1IN&SDA)==SDA){
				temp|=0x01;
			}
			else{
				temp&=~0x01;
			}
			//				__delay_cycles(7);
			P1OUT&=~SCL;		//CLK Low

			P1OUT |=SDA;
		}

		P1DIR |=SDA;				//Set output for Master ACK
		if (i==(txnum+rxnum-1)){		// make NACK
			P1OUT |=SDA;				//Data High for NACK
		}
		else{					//Master send ACK
			P1OUT &=~SDA;				//Data Low
			__delay_cycles(tide);

		}
		if (i==txnum){
			if (rxnum==100)
				rxnum=temp;
		}

		i2cbuf[i]=temp;


		__delay_cycles(tide);
		P1OUT|=SCL;				//CLK high
		__delay_cycles(tide*3);



		P1OUT &=~SCL;				//CLK Low

		__delay_cycles(tide);
		P1OUT &=~SDA;				//Data Low
		P1DIR&=~SDA;



	}
	P1DIR |=SDA;				//Set output
	P1OUT &=~SDA;				//Data low
	__delay_cycles(tide);
	P1OUT|=SCL;				//CLK high
	__delay_cycles(tide);
	P1OUT|=SDA;				// Data high: Stop bit
	return 0;


}






