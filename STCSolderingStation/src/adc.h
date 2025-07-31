#ifndef _ADC_H_
#define _ADC_H_

#include "config.h"

#define ADCTIM (*(unsigned char volatile xdata *)0xfea8)

void InitADC();
int ReadADCDat();
int ReadVinADCDat(); // 返回单位是 100mV
unsigned int GetVccVoltage();

#endif