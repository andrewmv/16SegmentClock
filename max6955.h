/*
 * max6955.h
 *
 *  Created on: Feb 15, 2013
 *      Author: Andrew
 */

#ifndef MAX6955_H_
#define MAX6955_H_

/*
 * ======= Configuration variables ========
 */
#define FULL_BRIGHTNESS 0x0f
#define HALF_BRIGHTNESS 0x00

//Register definitions
#define MAX_DECODE_MODE 0x01
#define MAX_INTENSITY 0X02
#define MAX_CFG 0x04
#define MAX_GPIO 0x05
#define MAX_PORT_CFG 0x06
#define MAX_TEST 0x07
#define MAX_DIGIT_TYPE 0x0c
#define MAX_DIGIT0 0x20
#define MAX_DIGIT1 0x21
#define MAX_DIGIT2 0x22
#define MAX_DIGIT3 0x23
#define MAX_DIGIT4 0x24
#define MAX_DIGIT5 0x25
#define MAX_DIGIT6 0x26
#define MAX_DIGIT7 0x27
#define MAX_PORTCONFIG 0x06
#define MAX_KEYA_MASK 0x08
#define MAX_KEYB_MASK 0x09
#define MAX_KEYC_MASK 0x0A
#define MAX_KEYD_MASK 0x0B
//Register bitmasks
#define MAX_CFG_RST BIT5
#define MAX_CFG_WAKE BIT1


char max_setreg(char addr, char reg, char val);
void max_init(char addr);


#endif /* MAX6955_H_ */
