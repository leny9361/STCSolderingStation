#include "delay.h"

void Delay1ms() //@11.0592MHz
{
	unsigned char data i, j;

	i = 15;
	j = 90;
	do
	{
		while (--j)
			;
	} while (--i);
}

void Delay10ms() //@11.0592MHz
{
	unsigned char data i, j;

	_nop_();
	_nop_();
	i = 144;
	j = 157;
	do
	{
		while (--j)
			;
	} while (--i);
}

void Delay300ms() //@11.0592MHz
{
	unsigned char data i, j, k;

	_nop_();
	i = 17;
	j = 213;
	k = 185;
	do
	{
		do
		{
			while (--k)
				;
		} while (--j);
	} while (--i);
}

void Delay1000ms() //@11.0592MHz
{
	unsigned char data i, j, k;

	i = 57;
	j = 27;
	k = 112;
	do
	{
		do
		{
			while (--k)
				;
		} while (--j);
	} while (--i);
}

void Delay510us() //@11.0592MHz
{
	unsigned char data i, j;

	i = 6;
	j = 121;
	do
	{
		while (--j)
			;
	} while (--i);
}

void Delay150ms()
{
	unsigned char i = 15;
	do
	{
		Delay10ms();
	} while (--i);
}

void DelaySec(unsigned char sec)
{
	while (sec--)
	{
		Delay1000ms();
	}
}