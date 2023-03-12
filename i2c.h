/*
 * i2c.h
 *
 *  Created on: Jun 6, 2022
 *      Author: jenkins
 */

#ifndef I2C_H_
#define I2C_H_

#include <msp430.h>

unsigned char *PTxData;                             // Pointer to TX data
unsigned char TxByteCtr;                            // number of bytes to TX

void i2c_init(void);
void i2c_write(unsigned char, unsigned char *, unsigned int);

#endif /* I2C_H_ */
