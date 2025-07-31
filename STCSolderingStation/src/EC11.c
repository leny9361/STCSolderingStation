#include "EC11.h"

extern void Ec11Proc(char v);

char pdata STATUS_MAC = 0; // 从高位开始记录
char pdata val = 0;

void InitEc11()
{
	IT0 = 0; // 使能INT0上升沿和下降沿中断
	EX0 = 1; // 使能INT0中断
}

void Check()
{
	if (val == -1)
	{
		if (STATUS_MAC == -1)
		{
			if (P32 == 0 && P33 == 0)
			{
				STATUS_MAC = -2;
			}
		}
	}
	else if (val == 1)
	{
		if (STATUS_MAC == 1)
		{
			if (P32 == 0 && P33 == 1)
			{
				STATUS_MAC = 2;
			}
		}
	}
}

void Int0Isr() interrupt 0
{
	if (STATUS_MAC == 0)
	{
		if (P32 == 0)
		{
			if (P33)
			{
				// 01
				STATUS_MAC = -1;
				val = -1;
			}
			else
			{
				// 00
				val = 1;
				STATUS_MAC = 1;
			}
		}
	}
	if (P32)
	{
		if (STATUS_MAC == 2 || STATUS_MAC == -2)
		{
			Delay1ms();
			if (val == -1)
			{
				if (P32 == 1 && P33 == 0)
				{
					Ec11Proc(val);
					STATUS_MAC = 0;
					val = 0;
				}
				else
				{
					STATUS_MAC = 0;
					val = 0;
				}
			}
			else if (val == 1)
			{
				if (P32 == 1 && P33 == 1)
				{
					Ec11Proc(val);
					STATUS_MAC = 0;
					val = 0;
				}
				else
				{
					STATUS_MAC = 0;
					val = 0;
				}
			}
		}
		else
		{
			//			STATUS_MAC = 0;
			//			val = 0;
		}
	}
}