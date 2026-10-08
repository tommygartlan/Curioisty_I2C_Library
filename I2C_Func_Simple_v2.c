
#include <xc.h>
#include <stdbool.h>

// ==========================================
// Low Level I2C Routines (From Appendix A)
// ==========================================

// Adapted from "i2c_init" [cite: 708]
void i2c_init(void) {
    
    RB1PPS = 0x11;    //using MSSP2  RB1 -> SCL2
    RB2PPS = 0x12;    //             RB2 -> SDA
    //Make pins digital in case students forget to make dig input
    ANSELBbits.ANSELB1 = 0; //Tnputs by default but not digital
    ANSELBbits.ANSELB2 = 0; //
    
    SSP2CON1 = 0x08; // Enable I2C Master mode [cite: 710]
    SSP2CON2 = 0x00; // Clear control bits
    SSP2STAT = 0x80; // Disable slew rate control (Standard Mode) [cite: 711]
    
    // Baud Rate Calculation for 100kHz at 8MHz Fosc
    // Formula: SSP2ADD = (Fosc / (4 * Baud)) - 1
    // (8,000,000 / 400,000) - 1 = 19
    SSP2ADD = 19;    // Set baud rate [cite: 711]
    
    PIR3bits.SSP2IF = 0; // Clear flag (Note: PIR3 for SSP2 on Q10)
    SSP2CON1bits.SSPEN = 1; // Enable MSSP
}

// Adapted from "i2c_start" [cite: 714]
void i2c_start(void) {
    PIR3bits.SSP2IF = 0;        // Clear flag
    while (SSP2STATbits.BF);    // Wait for idle condition.  BF(Busy Flag) = 1 means data transmit in progress
    SSP2CON2bits.SEN = 1;       // Initiate START condition
    while (!PIR3bits.SSP2IF);   // Wait for flag to be set
    PIR3bits.SSP2IF = 0;        // Clear flag
}

// Adapted from "i2c_repStart" [cite: 718]
void i2c_repStart(void) {
    PIR3bits.SSP2IF = 0;
    while (SSP2STATbits.BF);
    SSP2CON2bits.RSEN = 1;      // Initiate Repeated START
    while (!PIR3bits.SSP2IF);
    PIR3bits.SSP2IF = 0;
}

// Adapted from "i2c_stop" [cite: 720]
void i2c_stop(void) {
    PIR3bits.SSP2IF = 0;
    while (SSP2STATbits.BF);
    SSP2CON2bits.PEN = 1;       // Initiate STOP condition
    while (!PIR3bits.SSP2IF);
    PIR3bits.SSP2IF = 0;
}

// Adapted from "i2c_write" [cite: 724]
bool i2c_write(unsigned char i2cWriteData) {
    PIR3bits.SSP2IF = 0;
    while (SSP2STATbits.BF);
    SSP2BUF = i2cWriteData;     // Load SSPBUF [cite: 727]
    while (!PIR3bits.SSP2IF);   // Wait for flag
    PIR3bits.SSP2IF = 0;
    return (!SSP2CON2bits.ACKSTAT); // Return ACK status [cite: 729]
}

// Adapted from "i2c_read" [cite: 730]
unsigned char i2c_read(unsigned char ack) {
    unsigned char i2cReadData;
    PIR3bits.SSP2IF = 0;
    while (SSP2STATbits.BF);
    SSP2CON2bits.RCEN = 1;      // Enable receive mode [cite: 731]
    while (!PIR3bits.SSP2IF);
    PIR3bits.SSP2IF = 0;
    i2cReadData = SSP2BUF;      // Read SSPBUF [cite: 732]
    
    if (ack) {
        SSP2CON2bits.ACKDT = 0; // Transmit ACK [cite: 733]
    } else {
        SSP2CON2bits.ACKDT = 1; // Transmit NAK [cite: 734]
    }
    SSP2CON2bits.ACKEN = 1;     // Send acknowledge sequence
    while (!PIR3bits.SSP2IF);
    PIR3bits.SSP2IF = 0;
    return (i2cReadData);       // Return value
}

bool I2C_Write_Bytes(unsigned char address7Bit, const uint8_t *data, size_t dataLength) {
    // 1. Generate START
    i2c_start();

    // 2. Send Address + Write Bit (0)
    // Shift 7-bit address left by 1. LSB is 0 for Write.
    //if (!i2c_write((address7Bit << 1) | 0x00)) {
    if (!i2c_write((unsigned char)((address7Bit << 1) | 0x00))) {
        i2c_stop(); // NACK received, stop and exit
        return false; 
    }

    // 3. Send Data
    for (size_t i = 0; i < dataLength; i++) {
        if (!i2c_write(data[i])) {
            i2c_stop(); // NACK received
            return false; 
        }
    }

    // 4. Generate STOP
    i2c_stop();
    return true;
}


