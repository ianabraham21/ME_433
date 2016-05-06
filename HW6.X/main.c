#include "PIC32.h"
#include "ILI9163C.h"
#include "i2c_util.h"
#include <math.h>

#define SLAVE_ADDR 0b1101011 // slave address for IMU
// send a byte via spi and return the response

#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define OUT_TEMP_L 0x22 // start of the bit read

void write2LCD(char * str) {
    unsigned short i,j;
    unsigned short q = 0;
    unsigned short bitcolor;
    while (str[q] != 0){
        for (i=0; i<5; i++){
            for (j=0; j<8; j++){
                bitcolor = ASCII[str[q]-0x20][i] >> (j) & 1;
                if (1 == bitcolor ) {
                    LCD_drawPixel(i+5*q,j,BLACK);
                } else {
                    LCD_drawPixel(i+5*q,j,WHITE);
                }
            }
        }
        q++;
    }

}

int main() {

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
    if (databuff[0] == 0b01101001) {write2LCD(str);}
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < 240000000) {;}

    

    sprintf(str, "Now let's make a func"); 
    write2LCD(str);
}

