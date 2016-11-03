/*
 * display.c
 *
 * Functions related to rendering text on a MAX6955 based 16-segment display
 *
 *  Created on: Feb 15, 2013
 *      Author: Andrew
 */

#include "display.h"
#include "i2c.h"
#include "max6955.h"
#include <msp430g2452.h>

//Write the 8 character str to successive registers, starting at reg
char write8(char addr, char reg, char str[]) {
	_BIC_SR(GIE);	//Do not allow interrupts to be serviced while we're conducting an I2C transmission.
	char ack = I2C_ACK;

	do {
		i2c_start();
	/* Retry I2C start condition if we receive a NACK the first time.
	 * There seems to be a bug in the MAX6955 where the first write immediately following a read always returns a NACK.	 */
	} while (i2c_write8(addr << 1) != 0);	//Send slave address in 7 MSBs, and a '0' in the LSB to indicate 'write' mode

    ack = i2c_write8(reg);
    char i;
    for (i = 0; i < 8; i++) {	//Ignore CCS warning - countup instead of countdown here is appropriate in order to leverage I2C register autoincrement
    	ack |= i2c_write8(str[i]);
    }
    i2c_stop();

    _BIS_SR(GIE);	//Re-enable interrupts
    return ack;
}

//Display str, spanning modules as necessary
void display(char str[]) {
	char strpart[8];
	char i;
	char offset = 0;

	//Iterate i over modules
	for (i = 0; i < MODULE_COUNT; i++) {
		char j;
		//Iterate j over digits in module i
		for (j = 0; (j < 8) && (str[j + offset] != 0); j++) {
			strpart[j] = str[j + offset];
		}
		//Pad string with spaces after null
		for (; j < 8; j++) {
			strpart[j] = ' ';
		}
		write8(module_addr[i], MAX_DIGIT0, strpart);
		offset += 8;
	}

}

