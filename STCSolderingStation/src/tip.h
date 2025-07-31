#ifndef TIP_H
#define TIP_H

#include "config.h"
#include "sysconst.h"
#include "arduboy.h"
#include "oled.h"
#include "delay.h"
#include "string.h"

extern int CurrentTemp;

void TipScreen();
void ChangeTipScreen();
void CalibrationScreen();
void InputNameScreen();
void AddTipScreen();
void DeleteTipScreen();
void CalibrationInfo();

#endif