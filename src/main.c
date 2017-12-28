// have all the includes up here
#include "PIC32.h"
#include "LCD_utils.h"
#include "i2c_util.h" // this I need to get the accellerometer data
#include <math.h>

#define SLAVE_ADDR 0b1101011 // slave address for IMU
// send a byte via spi and return the response


#define WHO_AM_I 0x0F
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12

// this should start at the gyro/compass registers
#define OUTX_L_XL 0x28 // start of accel data
#define OUTX_L_G 0x22 // x axis angular rate low bit, angular data goes up to 0x28

void two_comp(unsigned char * data, char len, short * data_comb){
    /* helper function for two's compliment */
    int i = 0;
    for (i; i<len; i = i + 2){
        data_comb[i/2] = (data[i+1] << 8) | data[i] ;
    }

}

void init_pwm( void ){
    /*
    this initialies the pwm for both the motor control signals
    */

    // turn off any analog IO
    ANSELB = 0;
    // set the prescaler
    // 1:1 scaler
    RPB15Rbits.RPB15R = 0b0101;
    T3CONbits.TCKPS = 0; // this uses timer 3, may not want this
    // period = (PR3+1)*N
    PR3 = 3999;
    TMR3 = 0; // init timer 3 count
    OC1CONbits.OCTSEL = 1; // set oc1 to timer 3
    OC1CONbits.OCM = 0b110; // pwm mode w/o fault pin
    OC1RS = 0; // 0 % duty cycle
    OC1R = 0; // init before turning on oc1

    TRISBbits.TRISB13 = 0;
    LATBbits.LATB13 = 1;
    // should do the same for the other pwm port

    RPB8Rbits.RPB8R = 0b0101;
    OC2CONbits.OCTSEL = 1;
    OC2CONbits.OCM = 0b110;
    OC2RS = 0;
    OC2R = 0;

    TRISBbits.TRISB9 = 0;
    LATBbits.LATB9 = 1;

    // turn all the thngs on
    T3CONbits.ON = 1; // turn on timer 3
    OC1CONbits.ON = 1; // turn on oc1
    OC2CONbits.ON = 1;

}

// void init_pos_control( void ){
//     /*
//     this initializes the position control ISR which will
//     */
//     PR1 = 49999;
//     TMR1 = 0;
//     T1CONbits.TCKPS = 0b01; // N = 8
//     T1CONbits.ON = 1;
//     IPC1bits.T1IP = 5;
//     IPC1bits.T1IS = 3;
//     IFS0bits.T1IF = 0; // clear interrupts
//     IEC0bits.T1IE = 1; // enable interrupt
//
//     // enable the phase of the motor
//     TRISEbits.TRISE13 = 0;
//     LATEbits.LATE13 = 0;
//     TRISEbits.TRISE9 = 0;
//     LATEbits.LATE9 = 0;
//
// }

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
    // SPI1_init();
    // init LCD
    // LCD_init();
    // LCD_clearScreen(0x0); // clear the screen

    // init i2c
    i2c_master_setup();
    //checking the WHOAMI register
    unsigned char databuff[20];
    short filt_data[20];
    i2c_read(SLAVE_ADDR, WHO_AM_I, databuff, 1);

    // char str[100];
    // sprintf(str, "01101001");
    // if (databuff[0] == 0b01101001) {write2LCD(str, 0);}
    // _CP0_SET_COUNT(0);
    // while (_CP0_GET_COUNT() < 24000000) {;}

    // sprintf(str, "Works!");
    // write2LCD(str, 1);


    // write to the CTRL1_XL register
    databuff[0] = 0b10000000; // sets up the accel register + gyro
    i2c_write( SLAVE_ADDR, CTRL1_XL, databuff, 1);
    i2c_write( SLAVE_ADDR, CTRL2_G, databuff, 1);
    databuff[0] = 1 << 2; // this makes sense (sets IF_INC bit to 1)
    i2c_write (SLAVE_ADDR, CTRL3_C, databuff, 1);

    // init pwm
    init_pwm();
    OC1RS = 0;
    OC2RS = 0;
    short ang_vel;
    double accel[2];
    double theta;
    float control = 0;
    float Kp = 100.0, Kd = -0.1;

//     OC1RS = 2000;
//     OC2RS = 2000;

    while(1) {
        // let's try reading the gyroscope data
        // this doesn't have compass ... let's make one
        i2c_read( SLAVE_ADDR, OUTX_L_G, databuff, 16);
        two_comp(databuff, 16, filt_data);
        // get the x rot
        ang_vel = filt_data[0];
        accel[0] = filt_data[4];
        accel[1] = filt_data[5];

        theta = asin(accel[0]/sqrt(accel[0]*accel[0] + accel[1]*accel[1]));

        control = (float) Kp*theta + Kd*ang_vel;

        if (control > 100.0) {control = 100.0;}
        else if (control < -100.0) {control = -100.0;}

        if (control <= 0.0) {
            LATBbits.LATB9 = 0; LATBbits.LATB13 = 0;
            control = -1*control;
        }
        else if (control > 0.0){
            LATBbits.LATB9 = 1; LATBbits.LATB13 = 1;
        }
        else { LATBbits.LATB9 = 0; LATBbits.LATB13 = 0; control = 0;}
       OC1RS = (unsigned int) (control/100.0)*3999;
       OC2RS = (unsigned int) (control/100.0)*3999;
        // _CP0_SET_COUNT(0);
        // while (_CP0_GET_COUNT() < 24000) {;}
       /* DEBUGGING CODE
        sprintf(str,"x rot : %d     ",filt_data[0]);
        write2LCD(str, 3);
        sprintf(str,"y rot : %d     ",filt_data[1]);
        write2LCD(str, 4);
        sprintf(str,"z rot : %d     ",filt_data[2]);
        write2LCD(str, 5);

        sprintf(str,"x a : %d     ",filt_data[3]);
        write2LCD(str, 6);
        sprintf(str,"y a : %d     ",filt_data[4]);
        write2LCD(str, 7);
        sprintf(str,"z a : %d     ",filt_data[5]);
        write2LCD(str, 8);

        sprintf(str, "control : %f   ", control);
        write2LCD(str, 9);
        */
    }



}
