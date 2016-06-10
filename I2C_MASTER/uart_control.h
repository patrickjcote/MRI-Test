/*
 * uart_control.h
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */

#ifndef UART_CONTROL_H_
#define UART_CONTROL_H_
extern unsigned char tx_data_str[24], rx_data_str[24],rx_flag ,dec_str[7],eos_flag;
extern char dec_char[6];
void uart_init(int);
void uart_write_string(int,int);
char uart_get_char(int);
void uart_set_char(char,int);
void conv_hex_dec(int);
void unsigned_conv_hex_dec(int);
int conv_dec_hex (void);
void uart_write_fast_string(int, int);

#endif /* UART_CONTROL_H_ */
