#include "Timer0.h"
#include "EC11.h"
#include "config.h"

unsigned long golbal_millis = 0;

void InitTimer0()
{
	// 1ms
	AUXR |= 0x80; // 定时器时钟1T模式
	TMOD &= 0xF0; // 设置定时器模式
	TL0 = 0xCD;	  // 设置定时初始值
	TH0 = 0xD4;	  // 设置定时初始值
	TF0 = 0;	  // 清除TF0标志
	TR0 = 1;	  // 定时器0开始计时
	ET0 = 1;
}
unsigned long millis()
{
	return golbal_millis;
}

void Timer0Isr() interrupt 1
{
	golbal_millis++;

	if (STATUS_MAC != 0)
	{
		Check();
	}
}
