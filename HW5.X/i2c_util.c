#include "i2c_util.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_master_setup(void) {
  
    // this turns off the annoying analogue default 
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    // I think this is right
    I2C2BRG = 53;//90;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
                                    // look up PGD for your PIC32
    I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
    while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
    if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
        // ("I2C2 Master: failed to receive ACK\r\n");
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
    I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
    while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}


void i2c_write(unsigned char address, unsigned char reg, 
        unsigned char * data, char len){ // write data from dev

    i2c_master_start(); // start the first bit
    i2c_master_send(address << 1 | 0); // slave address left bit shifted and 0 for write
    i2c_master_send(reg); // send a command to address the register

    char i; // iterator for the data
    for (i=0; i<len; i++){
        i2c_master_send(data[i]); // deferenced data
    }
    i2c_master_stop(); // stop bit
}

void i2c_read(unsigned char address, unsigned char reg, 
        unsigned char * data, char len){ // read data from dev 
    // some i2c stuff that I think will work 
    i2c_master_start(); // send the start bit
    i2c_master_send((address << 1) | 0); // left shift address or'ed with 0 for write 1 for read
    i2c_master_send(reg); // write to the register
    i2c_master_restart(); // start again
    i2c_master_send((address << 1) | 1);

    // read first message to present annoyances with reading multiple bytes
    data[0] = i2c_master_recv();    

    // have the loop start at 1 to prevent sending an addition ack 
    char i;
    for (i = 1; i < len; i++){
        if (i == 1) { i2c_master_ack(0); }
        data[i] = i2c_master_recv();
        i2c_master_ack(0);
    }
    // if len == 1 then the loop should skip and end the read
    i2c_master_ack(1);
    i2c_master_stop();

}







