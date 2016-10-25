/*
 * spi_bb.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Bryce
 */

#ifndef SPI_BB_H_
#define SPI_BB_H_
void spi_bb_init(void);
void spi_bb_xfer(char *,char * ,int,int);
int swap_endian(int,int);


#endif /* SPI_BB_H_ */
