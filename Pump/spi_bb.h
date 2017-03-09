/*
 * spi_bb.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Bryce
 */

#ifndef PUMPS_AND_VALVE_SPI_BB_H_
#define PUMPS_AND_VALVE_SPI_BB_H_
void spi_bb_init(void);
void spi_bb_xfer(char *,char * ,int,int);
int swap_endian(int,int);


#endif /* PUMPS_AND_VALVE_SPI_BB_H_ */
