#include <msp430.h> 
#include "i2c.h"
#include "uart_control.h"
/*
 * main.c
 *
 * This code is an I2C master specifically for the 2016 NRMC team.  This device acts as the master to data feedback systems throughout the robot.  Each device has a different address.
 * See additional documentation for more information regarding each individual slave device
 *
 * This device acts as a bridge between UART and an I2C bus.  The pins used are as follows
 *
 * P1.1  UART TX
 * P1.2  UART RX
 * P1.6  I2C Master SCL
 * P1.7  I2C Master SDA
 *
 * The UART is connected at 115200 Baud, but can be changed if desired.  The computer communicating with this device is in control  Simply send data via the serial line.  Each command is followed by a carriage return to execute
 * Here are the commands that may be of interest
 *  I   > This identifies this device.  It should return most of the string "MASTERI2C"
 *  B   >  This broadcasts to all possible slaves and see if it responds.  It returns all available slave devices addresses
 *  #  >followed by the 7 bit address (in hex) of the slave device.  This returns the current filled buffer of the slave.  The first byte received is the total number of bytes that will be transmitted back
 * 		>  Each pair of bytes is the MSB and LSB of an unsigned value.  The meaning of the unsigned value will be given in the individual slave device values
 * 		>  For example  I may send the command:  #48 (followed by return).  I may receive back the five bytes (these are in decimal format):  5 3 24 65 92.  This gives me two 16 bit unsigned values of 792 and 16732
 * 		>  The meaning of these values may not be clear until you send review what you should be getting.  In some cases you may need to switch from an unsigned value to a signed (2's compliment).  The slave device converts
 * 		>  values to a string and you receive three strings separated by carriage returns.  You would receive the following:
 * 				3
 * 				00792
 * 				16732
 * 	*  >followed by the 7 bit address (in hex) of the slave device then followed by a two byte value (MSB then LSB).  Though this protocol may change as these devices mature currently there are few
 * 	   >devices that receive an input.  This sends the two bytes to the slave as desired.  You wll not need to send two bytes, but a string representing two bytes.  For example if you wanted to send the value of 196 to
 * 	   >the slave device 48 you would send the string *4800196 followed by carriage return.  Note the leading zeros.  You must always send five characters for that
 *
 */

int conv_char_hex(char *,int );


int main(void) {
	volatile char led_buf[4],identify[]="MASTERI2C";
	unsigned int TXVAL=0;
	char i2cbuf[64];  //Create 64 byte local buffer for data gransfer
	volatile int taddress, temp,k,n,loopval;
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	uart_init(8);								// Initialize uart at 115200 baud

	i2c_init();									// initialize I2C Master on pins 1.7 and 1.6
	__bis_SR_register(GIE);						// Enable global interrupts (for UART)
	while(1){
		if (eos_flag){				// Check to see if a carriage return as been sent

			eos_flag=0;
			if (rx_data_str[0]=='I'){		// Return identifying information for this device
				for (n=0;n<9;n++)
					tx_data_str[n]=identify[n];
				uart_write_string(0,9);
			}
			else if (rx_data_str[0]=='#'){			// Grab data from a slave device
				i2cbuf[0]=conv_char_hex(rx_data_str+1,2);		//Convert address data to first byte
				i2cbuf[0]<<=1;
				i2cbuf[0]++;									// Make it a "read" address
				k=i2c_rx_bb(i2cbuf,1,-1);							// read I2C bus with indefinite number of return bytes (-1)
				if (k==0){
					loopval=(i2cbuf[1]-1)/2;
					i2cbuf[1]=0;
					tx_data_str[0]=loopval+0x30;
					uart_write_string(0,1);
					for (n=0;n<loopval;n++){						// Convert returned data into set of strings
						temp=i2cbuf[n*2+2];
						i2cbuf[n*2+2]=0;
						temp<<=8;
						temp&=~0xFF;
						temp|=(i2cbuf[n*2+3]&0xFF);
						i2cbuf[n*2+3]=0;
						unsigned_conv_hex_dec(temp);
						temp=0;
						for (k=0;k<6;k++)
							tx_data_str[k]=dec_str[k];
						uart_write_string(0,6);						// Send each string
					}
				}
			}
			else if (rx_data_str[0]=='*'){						// Send desired values to I2C Slave
				if (rx_flag>=9){
					i2cbuf[0]=conv_char_hex(rx_data_str+1,2);
					i2cbuf[0]<<=1;
					TXVAL=0;
					for (k=0;k<5;k++){							// Convert string of characters into a single 2 byte word
						TXVAL*=10;
						TXVAL+=(rx_data_str[k+3]-0x30);
					}
					i2cbuf[2]=(TXVAL&0xff);						// Split word into two bytes
					i2cbuf[1]=((TXVAL>>8)&0xff);
					i2c_rx_bb(i2cbuf,rx_flag-3,0);				// Transmit bytes via I2C bus
				}
			}
			else if (rx_data_str[0]=='B'){						//Broadcast to all available slaves
				for (k=0;k<0x80;k++){
					i2cbuf[0]=(k<<1)+1;
					n=i2c_rx_bb(i2cbuf,1,1);
					if(n==0){  //Slave exists:  Yay!
						n=((k>>4)&0x0F);
						if (n>9)
							tx_data_str[0]=n+0x5B;
						else
							tx_data_str[0]=n+0x30;
						n=(k&0x0F);
						if (n>9)
							tx_data_str[1]=n+0x5B;
						else
							tx_data_str[1]=n+0x30;
						uart_write_string(0,2);

					}

				}
			}
			rx_flag=0;
		}

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


