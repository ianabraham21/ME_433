#ifndef SPI_UTIL_H__
#define SPI_UTIL_H__
#include <xc.h>
#include <sys/attribs.h>

#define CS LATBbits.LATB7       // chip select pin

void initSPI1(void);
void setVoltage(char, unsigned char);

unsigned char spi_io(unsigned char);

#endif