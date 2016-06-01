#include "LCD_utils.h"
// code to write to the LCD
void write2LCD(char * str, unsigned short column) {
    unsigned short i,j;
    unsigned short q = 0;
    unsigned short bitcolor;
    while (str[q] != 0){
        for (i=0; i<5; i++){
            for (j=0; j<8; j++){
                bitcolor = ASCII[str[q]-0x20][i] >> (j) & 1;
                if (1 == bitcolor ) {
                    LCD_drawPixel(i+5*q,j+column*8,BLACK);
                } else {
                    LCD_drawPixel(i+5*q,j+column*8,WHITE);
                }
            }
        }
        q++;
    }

}
