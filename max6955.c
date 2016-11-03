/*
 * max6955.c
 *
 *	Functions related to interfacing with the MAX6955 via I2C
 *  Created on: Feb 15, 2013
 *      Author: Andrew
 */

#include "max6955.h"
#include "i2c.h"
#include <msp430g2452.h>

//Perform a full i2c transaction to set REG to VAL
char max_setreg(char addr, char reg, char val) {
	_BIC_SR(GIE);
	char ack = I2C_ACK;

//	do {
		i2c_start();
//	} while (i2c_write8(addr << 1) != 0);
		i2c_write8(addr << 1);

    ack |= i2c_write8(reg);
    ack |= i2c_write8(val);
    i2c_stop();

    _BIS_SR(GIE);
    return ack;
}

void max_init(char addr) {
	max_setreg(addr, MAX_DIGIT_TYPE, 0x0);	//Put display in 16-segment mode
	max_setreg(addr, MAX_DECODE_MODE, 0xff);
//	max_setreg(addr, MAX_TEST, 0x01);			//Set test mode (mainly for hardware debugging)
	max_setreg(addr, MAX_TEST, 0X00);			//Disable test mode
	max_setreg(addr, MAX_CFG, 0x21);			//Reset and disable shutdown mode (enter normal operation mode)
	max_setreg(addr, MAX_INTENSITY, FULL_BRIGHTNESS);	    //Global brightness
}



