#ifndef _T12TIMER_H
#define _T12TIMER_H

#include "config.h"
#include "sysconst.h"
#include "arduboy.h"
#include "oled.h"
#include "delay.h"
#include "string.h"

void TimerScreen();
unsigned int InputScreen(char code *xdata Items[]);

#endif