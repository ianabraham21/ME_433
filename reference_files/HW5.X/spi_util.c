#include "spi_util.h"
void initSPI1() {
    // set RB13 pin to be SDO1
    RPB13Rbits.RPB13R = 0b0011;

    // set the chip select pin to be an output pin
    TRISBbits.TRISB7 = 0;
    CS = 1; // sets chip select to high

    // setup spi1
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 9999;            // baud rate to 10 MHz [SPI1BRG = (40000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 1
}

void setVoltage(char channel, unsigned char voltage){
    unsigned short dat_buf = 0x0;
    dat_buf |= channel << 15;
    dat_buf |= 0x7 << 12;
    dat_buf |= voltage << 4;

    CS = 0; // set chip select low
    spi_io((dat_buf & 0xFF00) >> 8);
    spi_io(dat_buf & 0x00FF);
    CS = 1;
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}