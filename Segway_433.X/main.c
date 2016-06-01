// have all the includes up here
#include "PIC32.h"
#include "LCD_utils.h"
#include "i2c_util.h" // this I need to get the accellerometer data
#include "LCD_utils.h"
#include <math.h>

#define SLAVE_ADDR 0b1101011 // slave address for IMU
// send a byte via spi and return the response


#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12

// this should start at the gyro/compass registers
#define OUT_TEMP_L 0x22 // start of the bit read
#define OUTX_L_XL 0x28 // start of accel data

// code for two's compliment
void two_comp(unsigned char * data, char len, short * data_comb){

    int i = 0;
    for (i; i<len; i = i + 2){
        data_comb[i/2] = (data[i+1] << 8) | data[i] ;
    }

}

// main loop
int main(){
    // basic starting init for the pic
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

    // init SPI
    SPI1_init();
    // init LCD
    LCD_init();
    LCD_clearScreen(0x0); // clear the screen

    // init i2c
    i2c_master_setup();
    //checking the WHOAMI register
    unsigned char databuff[10];
    short filt_data[10];
    i2c_read(SLAVE_ADDR, WHO_AM_I, databuff, 1);

    char str[100];
    sprintf(str, "01101001");
    if (databuff[0] == 0b01101001) {write2LCD(str, 0);}
    // _CP0_SET_COUNT(0);
    // while (_CP0_GET_COUNT() < 24000000) {;}

    sprintf(str, "Works!");
    write2LCD(str, 1);


    // write to the CTRL1_XL register
    databuff[0] = 0b10000000; // sets up the accel register + gyro
    i2c_write( SLAVE_ADDR, CTRL1_XL, databuff, 1);
    i2c_write( SLAVE_ADDR, CTRL2_G, databuff, 1);
    databuff[0] = 1 << 2; // this makes sense (sets IF_INC bit to 1)
    i2c_write (SLAVE_ADDR, CTRL3_C, databuff, 1);

    // let's try reading the temperature data
    i2c_read( SLAVE_ADDR, OUT_TEMP_L, databuff, 14);
    while(1) {

        // let's try reading the temperature data
        i2c_read( SLAVE_ADDR, OUTX_L_XL, databuff, 6);
        two_comp(databuff, 14, filt_data);
        sprintf(str,"x : %d     ",filt_data[0]);
        write2LCD(str, 3);
        sprintf(str,"y : %d     ",filt_data[1]);
        write2LCD(str, 4);
        sprintf(str,"z : %d     ",filt_data[2]);
        write2LCD(str, 5);
    }



}
