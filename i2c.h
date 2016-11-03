#ifndef I2C_H
#define I2C_H

//Pin mappings
#define SDA  BIT7 //P1.7
#define SCL  BIT6 //P1.6

//I2C ACK return codes
#define I2C_ACK 0x00
#define I2C_NACK 0x01

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_write8(unsigned char c);
unsigned char i2c_read8();

#endif

