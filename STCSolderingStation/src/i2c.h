#ifndef _I2C_H_
#define _I2C_H_

#include "config.h"

void Init_I2C(void);
void I2CStart();
bit I2CWrite(unsigned char dat);
unsigned char I2CReadACK();
unsigned char I2CReadNAK();
void I2CStop();
void UnInit_I2C(void);

#endif