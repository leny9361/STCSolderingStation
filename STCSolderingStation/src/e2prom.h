#ifndef E2PROM_H
#define E2PROM_H

#include "config.h"

void IapErase();
char Read(int addr);
void Write(int addr, char dat);

#endif