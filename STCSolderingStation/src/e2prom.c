#include "e2prom.h"

#define ADDR 0x000
void IapProgram(int addr, char dat);
void IapErase(int addr);

void IapIdle()
{
	IAP_CONTR = 0;	  // 关闭IAP功能
	IAP_CMD = 0;	  // 清除命令寄存器
	IAP_TRIG = 0;	  // 清除触发寄存器
	IAP_ADDRH = 0x80; // 将地址设置到非IAP区域
	IAP_ADDRL = 0;
}

char Read(int addr)
{
	char dat;
	addr += ADDR;
	IAP_CONTR = 0x80;	   // 使能IAP
	IAP_TPS = 12;		   // 设置等待参数12MHz
	IAP_CMD = 1;		   // 设置IAP读命令
	IAP_ADDRL = addr;	   // 设置IAP低地址
	IAP_ADDRH = addr >> 8; // 设置IAP高地址
	IAP_TRIG = 0x5a;	   // 写触发命令(0x5a)
	IAP_TRIG = 0xa5;	   // 写触发命令(0xa5)
	_nop_();
	dat = IAP_DATA; // 读IAP数据
	IapIdle();		// 关闭IAP功能

	return dat;
}

void Write(int addr, char dat)
{
	IapProgram(addr + ADDR, dat);
}

void IapProgram(int addr, char dat)
{
	IAP_CONTR = 0x80;	   // 使能IAP
	IAP_TPS = 12;		   // 设置等待参数12MHz
	IAP_CMD = 2;		   // 设置IAP写命令
	IAP_ADDRL = addr;	   // 设置IAP低地址
	IAP_ADDRH = addr >> 8; // 设置IAP高地址
	IAP_DATA = dat;		   // 写IAP数据
	IAP_TRIG = 0x5a;	   // 写触发命令(0x5a)
	IAP_TRIG = 0xa5;	   // 写触发命令(0xa5)
	_nop_();
	IapIdle(); // 关闭IAP功能
}

void IapErase()
{
	IAP_CONTR = 0x80;	   // 使能IAP
	IAP_TPS = 12;		   // 设置等待参数12MHz
	IAP_CMD = 3;		   // 设置IAP擦除命令
	IAP_ADDRL = ADDR;	   // 设置IAP低地址
	IAP_ADDRH = ADDR >> 8; // 设置IAP高地址
	IAP_TRIG = 0x5a;	   // 写触发命令(0x5a)
	IAP_TRIG = 0xa5;	   // 写触发命令(0xa5)
	_nop_();			   //
	IapIdle();			   // 关闭IAP功能
}