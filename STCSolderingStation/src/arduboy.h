#ifndef _ARDUBOY_H
#define _ARDUBOY_H

#include "config.h"
#include "i2c.h"

#define true 1
#define false 0

#define WIDTH 128
#define HEIGHT 64

#define WHITE 1
#define BLACK 0

// typedef bit bool;

void InitArduboy();
void Clear();
int min(int val1, int val2);
int max(int val1, int val2);
void DrawPixel(unsigned char x, unsigned char y, bit color);
void DrawFastVLine(unsigned char x, unsigned char y, unsigned char h, bit color);
void Print(unsigned char x, unsigned char y, unsigned char *str, unsigned char cSize);
void FillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, bit color);
void DrawSlowXYBitmap(unsigned char x, unsigned char y, unsigned char *bitmap, unsigned char w, unsigned char h, bit color);

// typedef struct
//{
//	bool (*nextFrame)(void);
// } ARDUBOY;

// extern ARDUBOY arduboy;
extern char xdata sBuffer[1024];
#endif