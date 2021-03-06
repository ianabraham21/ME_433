#ifndef PIC32__H__
#define PIC32__H__

#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = 0x2 // no debugging
#pragma config JTAGEN = 0x0 // no jtag
#pragma config ICESEL = 0x3 // use PGED1 and PGEC1
#pragma config PWP = 0x3F // no write protect
#pragma config BWP = 0x0 // no boot write protect
#pragma config CP = 0x1 // no code protect

// DEVCFG1
#pragma config FNOSC = 0x3 // use primary oscillator with pll
#pragma config FSOSCEN = 0x0 // turn off secondary oscillator
#pragma config IESO = 0x0 // no switching clocks
#pragma config POSCMOD = 0x2 // high speed crystal mode
#pragma config OSCIOFNC = 0x1 // free up secondary osc pins
#pragma config FPBDIV = 0x0 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = 0x3 // do not enable clock switch
#pragma config WDTPS = 0x0 // slowest wdt
#pragma config WINDIS = 0x1 // no wdt window
#pragma config FWDTEN = 0x0 // wdt off by default
#pragma config FWDTWINSZ = 0x3 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = 0x1 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 0x7 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = 0x1 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 0x1 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = 0x0 // USB clock on

// DEVCFG3
#pragma config USERID = 0xA26B // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0x0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0x0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 0x1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 0x1 // USB BUSON controlled by USB module

#endif
