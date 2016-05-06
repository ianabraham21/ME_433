#include "PIC32.h"
#include "ILI9163C.h"
#include <math.h>

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



    char str[100];
    unsigned short i,j;

    sprintf(str, "Now let's make a func"); 
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