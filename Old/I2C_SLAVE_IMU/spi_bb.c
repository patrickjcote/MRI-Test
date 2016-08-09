/*
 * spi_bb.c
 *
 *  Created on: Dec 19, 2014
 *      Author: Bryce
 */

#include  "msp430.h"
#define DSPI 10

void spi_bb_init(){
	P1DIR |= BIT0 + BIT5;                     //
	  P1SEL = BIT1 + BIT2 + BIT4;
	  P1SEL2 = BIT1 + BIT2 + BIT4;
	  UCA0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
	  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	  UCA0BR0 |= 0x12;                          // /2
	  UCA0BR1 = 0;                              //
	  UCA0MCTL = 0;                             // No modulation
	  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
//	  IE2 |= UCA0RXIE;                          // Enable USCI0 RX interrupt

}

int swap_endian(int val,int nbits){
	volatile int outval=0;
	volatile int r=0;
	for (r=0;r<nbits;r++){
		outval<<=1;
		outval&=~BIT0;
		if (val&BIT0)
			outval|=BIT0;
		val>>=1;

	}

	return outval;
}

void spi_bb_xfer(char *in_data,char *out_data,int num,int lendian){
	volatile int k,i;
	volatile char inval,outval;
	__delay_cycles(DSPI);
	P1OUT&=~BIT5;
	__delay_cycles(DSPI);
	for (k=0;k<num;k++){
		outval=*(in_data+k);
		if (lendian)
			outval=swap_endian(outval,8);
		while (!(IFG2 & UCA0TXIFG));
		UCA0TXBUF = outval;
		while (!(IFG2 & UCA0RXIFG));
		inval=UCA0RXBUF;
		if (lendian)
			inval=swap_endian(inval,8);
		*(out_data+k-1)=inval;
//		while (!(IFG2 & UCA0TXIFG));
		__delay_cycles(DSPI);
	}
}
