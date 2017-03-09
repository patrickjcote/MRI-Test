/*
 * serial_handler.h
 *
 *  Created on: Mar 3, 2016
 *      Author: BHill
 */

#ifndef REELS_SERIAL_HANDLER_H_
#define REELS_SERIAL_HANDLER_H_

extern unsigned char tx_data_str[24], rx_data_str[24],rx_flag ,dec_str[7],eos_flag;
extern char dec_char[6];
void uart_init(int);
void uart_write_string(int,int);
char uart_get_char(int);
void uart_set_char(char,int);
void conv_hex_dec(int);
void unsigned_conv_hex_dec(int);
int conv_dec_hex (void);
void  i2c_slave_init(int);
//void uart_write_fast_string(int, int);
extern unsigned char i2cTXData[64],i2cRXData[64];
extern volatile int i2cTXData_ptr,i2cRXData_ptr,i2crxflag;
extern volatile int i2cmode;


#endif /* REELS_SERIAL_HANDLER_H_ */
