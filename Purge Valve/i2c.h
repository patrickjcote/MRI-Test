/*
 * i2c.h
 *
 *  Created on: Jul 28, 2014
 *      Author: BHill
 */

#ifndef PUMPS_AND_VALVE_I2C_H_
#define PUMPS_AND_VALVE_I2C_H_
void i2c_init(void);
int i2c_rx_bb(char *,int, int);
void wait_burn(int);
//extern volatile char i2cbuf[16];



#endif /* PUMPS_AND_VALVE_I2C_H_ */
