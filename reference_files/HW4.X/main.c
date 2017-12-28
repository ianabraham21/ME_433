#include "PIC32.h"
#include "i2c_util.h"
#include <math.h>

#define CS LATBbits.LATB7       // chip select pin
#define SLAVE_ADDR 0x20
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


    i2c_master_setup();
    initSPI1(); // initialize spi1
    unsigned char volts = 0;
    char channel = 0;
    
    // set the voltage to 0 for now
    setVoltage(channel, volts);
    
    // sine wave
    unsigned char sinwave[1000];
    int i = 0, center = 255/2, A = 255/2; // square wave
    for (i = 0; i < 1000; i++){
        sinwave[i] = (unsigned char)  center + A * sin( 2 * 3.14 * i / (1000) );
    }

    // triangle wave things
    unsigned char t_wave[1000];
    i = 0;
    for (i = 0; i < 1000; i++){
        t_wave[i] = (unsigned char) 255 * i /1000;
    }

    i = 0;

    // some i2c stuff that I think will work 
           i2c_master_start(); // send the start bit
           i2c_master_send(0x40); // left shift address or'ed with 0 for write
           i2c_master_send(0x00); // write to the GPIO pins
           i2c_master_send(0x00); // make GPIO pin 1, 4 , and 6 latch to high
           i2c_master_stop();

           i2c_master_start();
           i2c_master_send(0x40); // left shift address or'ed with 0 for write
           i2c_master_send(0x0A); // write to the GPIO pins
           i2c_master_send(0b010); // make GPIO pin 1, 4 , and 6 latch to high
           i2c_master_stop();

           // i2c_master_send(SLAVE_ADDR << 1); // left shift address or'ed with 0 for write
           // i2c_master_send(0x09); // write to the GPIO pins
           // i2c_master_send(0xFF); // make GPIO pin 1, 4 , and 6 latch to high
           // i2c_master_stop();


    while(1) {
           // i2c_master_start(); // send the start bit
           // i2c_master_send(SLAVE_ADDR << 1); // left shift address or'ed with 0 for write
           // i2c_master_send(0x00); // write to the GPIO pins
           // i2c_master_send(0x00); // make GPIO pin 1, 4 , and 6 latch to high
           // i2c_master_stop();

           // i2c_master_send(SLAVE_ADDR < 1|0); // left shift address or'ed with 0 for write
           // i2c_master_send(0x0A); // write to the GPIO pins
           // i2c_master_send(0xFF); // make GPIO pin 1, 4 , and 6 latch to high
           // i2c_master_stop();

	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		// remember the core timer runs at half the CPU speed
        // i = 0;
        if(_CP0_GET_COUNT()>24000){ // this is one second 24000000
            i++;
            setVoltage(0, sinwave[i]);
            setVoltage(1, t_wave[i]);
            _CP0_SET_COUNT(0);
         }
        if (i > 1000){i = 0;}

    }
    
    
}