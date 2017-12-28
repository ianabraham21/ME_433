#include "PIC32.h"

#define CS LATBbits.LATB8       // chip select pin

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void initSPI1() {

    // set RB13 pin to be SDO1
    RPB13Rbits.RPB13R = 0b0011;

    // set the chip select pin to be an output pin
    TRISBbits.TRISB8 = 0;
    CS = 1; // sets chip select to high

    // setup spi4
    SPI1CON = 0;              // turn off the spi module and reset it
    SPI1BUF;                  // clear the rx buffer by reading from it
    SPI1BRG = 0x3;            // baud rate to 10 MHz [SPI1BRG = (40000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0;  // clear the overflow bit
    SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1;    // master operation
    SPI1CONbits.ON = 1;       // turn on spi 1

}


void setVoltage(char channel, char voltage){
    unsigned short dat_buf = 0x0;
    dat_buf != channel << 15;
    dat_buf != 0b111 << 12;
    dat_buf != voltage << 4;

    CS = 0; // set chip select low
    spi_io((dat_buf & 0xFF00) >> 8);
    spi_io(dat_buf & 0x00FF);
    CS = 1;
}

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;    
    __builtin_enable_interrupts();

    initSPI1(); // initialize spi1

    while(1) {
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		// remember the core timer runs at half the CPU speed
         
        // i = 0;
         if(_CP0_GET_COUNT()>12000){
             LATBbits.LATB8 = !LATBbits.LATB8;
             _CP0_SET_COUNT(0);
         }
    }
    
    
}