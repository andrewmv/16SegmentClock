/*
 * display.h
 *
 *  Created on: Feb 15, 2013
 *      Author: Andrew
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define MODULE_COUNT 2

extern char module_addr[];
char write8(char addr, char reg, char str[]) ;
void display(char str[]);

#endif /* DISPLAY_H_ */
