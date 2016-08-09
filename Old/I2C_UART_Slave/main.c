#include <msp430.h>
#include "serial_handler.h"
/*
 * main.c
 */



int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	volatile char identify[]="Identify_your_device";
	volatile int n;
	__delay_cycles(50000);
	i2c_slave_init(0x48);  //Set slave address to 0x48
	uart_init(4);   // set uart baud rate to 9600


	__bis_SR_register(GIE);
	i2cTXData[0]=3;			//Dummy values first input to i2cTXData
	i2cTXData[1]=10;
	i2cTXData[2]=12;
	while(1){
		if (eos_flag){					// if data received from the UART
			rx_flag=0;
			eos_flag=0;
			if (rx_data_str[0]=='I'){	// If it is the identifying character return the device ID
				for (n=0;n<9;n++)
					tx_data_str[n]=identify[n];
				uart_write_string(0,9);
			}
		}
		if (i2crxflag){					// if data received as the I2C slave
			__delay_cycles(50000);
			i2crxflag=0;

		}



	}
}








