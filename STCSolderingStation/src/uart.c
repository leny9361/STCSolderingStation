#include "uart.h"

bit busy;
char wptr;
char rptr;
char uartbuf[8];

void UartIsr() interrupt 4
{
	if (TI)
	{
		TI = 0;
		busy = 0;
	}
	if (RI)
	{
		RI = 0;
		uartbuf[wptr++] = SBUF;
		wptr &= 0x07;
	}
}

void UartInit()
{
	SCON = 0x50;
	T2L = BRT;
	T2H = BRT >> 8;
	AUXR |= 0x15;
	wptr = 0x00;
	rptr = 0x00;
	busy = 0;
}

void UartSend(char dat)
{
	while (busy)
		;
	busy = 1;
	SBUF = dat;
}
void UartSendStr(char *p, unsigned int len)
{
	if (len)
	{
		while (len--)
		{
			UartSend(*p++);
		}
	}
	else
	{
		while (*p)
		{
			UartSend(*p++);
		}
	}
}
// void UartSendStr(char *p)
//{
//     while (*p)
//     {
//         UartSend(*p++);
//     }
// }
// void UartSendStrByLength(char *p,unsigned char len)
//{
//	unsigned char i = 0;
//   for(;i < len; i++)
//	{
//		UartSend(*p++);
//	}
// }
