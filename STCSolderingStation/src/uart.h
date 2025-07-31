#ifndef _UART_H_
#define _UART_H_

#include "config.h"
#define FOSC 11059200UL
#define BRT (65536 - FOSC / 115200 / 4)

void UartInit();
void UartSend(char dat);
// void UartSendStr(char *p);
void UartSendStr(char *p, unsigned int len);
// void UartSendStrByLength(char *p,unsigned char len);

extern char uartbuf[8];
extern char wptr;
#endif