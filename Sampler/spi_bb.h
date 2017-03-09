/*
 * spi_bb.h
 *
 *  Created on: Dec 19, 2014
 *      Author: Bryce
 */

#ifndef SAMPLER_SPI_BB_H_
#define SAMPLER_SPI_BB_H_
void spi_bb_init(void);
void spi_bb_xfer(char *,char * ,int,int);
int swap_endian(int,int);


#endif /* SAMPLER_SPI_BB_H_ */
