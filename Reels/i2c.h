/*
 * i2c.h
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */

#ifndef REELS_I2C_H_
#define REELS_I2C_H_
void i2c_init(void);
int i2c_rx_bb(char *,int, int);
void wait_burn(int);
//extern volatile char i2cbuf[16];



#endif /* REELS_I2C_H_ */
