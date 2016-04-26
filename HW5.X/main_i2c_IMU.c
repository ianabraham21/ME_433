#include "PIC32.h"
#include "i2c_util.h"
#include "spi_util.h"
#include <math.h>

#define SLAVE_ADDR 0b1101011 // slave address for IMU
// send a byte via spi and return the response

#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12

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
   // i2c_master_start(); // send the start bit
   // i2c_master_send((SLAVE_ADDR << 1) | 0); // left shift address or'ed with 0 for write 1 for read
   // i2c_master_send(0x0F); // write to the register
   // i2c_master_restart(); // start again
   // i2c_master_send((SLAVE_ADDR << 1) | 1);
   // char r = i2c_master_recv();
   // i2c_master_ack(1);
   // i2c_master_stop();
   
   // // setting up the accel
   // i2c_master_start(); // start bit
   // i2c_master_send((SLAVE_ADDR << 1) | 0);
   // i2c_master_send(0x10); // the CTRL1_XL register
   // i2c_master_send(0b10000000); // this looks about right
   // i2c_master_stop();

    CS = 0;

    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < 24000000) { ; }

    // checking the WHOAMI register
    unsigned char databuff[10];
    i2c_read(SLAVE_ADDR, WHO_AM_I, databuff, 1);
    if (databuff[0] == 0b01101001) {CS = 1;}

    // write to the CTRL1_XL register
    databuff[0] = 0b10000000; // sets up the accel register + gyro
    i2c_write( SLAVE_ADDR, CTRL1_XL, databuff, 1);
    i2c_write( SLAVE_ADDR, CTRL2_G, databuff, 1);
    databuff[0] = 1 << 2; // this makes sense (sets IF_INC bit to 1)
    i2c_write (SLAVE_ADDR, CTRL3_C, databuff, 1);

    // let's try reading the temperature data


    while(1) { ; }
        
}




