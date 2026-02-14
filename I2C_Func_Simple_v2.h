/* 
 * File:   I2C_Func_Simple.h
 * Author: gartlant
 *
 * Created on December 11, 2025, 12:38 PM
 */

#ifndef I2C_FUNC_SIMPLE_V2_H
#define	I2C_FUNC_SIMPLE_V2_H





void i2c_init(void);
void i2c_start(void);
void i2c_repStart(void);
void i2c_stop(void);
bool i2c_write(unsigned char i2cWriteData);
unsigned char i2c_read(unsigned char ack);
bool I2C_Write_Bytes(unsigned char address7Bit, const uint8_t *data, size_t dataLength);










#endif	/* I2C_FUNC_SIMPLE_H */

