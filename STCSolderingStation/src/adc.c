#include "adc.h"
#include "delay.h"

unsigned int GetVccVoltage();
unsigned long ReadADCNum(unsigned int vcc, unsigned char ch); // 0：转换烙铁头电压，1：转换Vin电压

void InitADC()
{
	//	P0M0 = 0x00;
	//	P0M1 = 0x01;
	P_SW2 |= 0x80;
	ADCTIM = 0x3F; // 设置ADC内部时序
	P_SW2 &= 0x7F;
	ADCCFG = 0x2F;	  // 设置ADC时钟为系统时钟/2/16，并且结果右边对齐
	ADC_CONTR = 0x80; // 使能ADC模块
}
int ReadADCDat()
{
	unsigned int i = 0;
	unsigned long vcc = 0, res = 0;
	unsigned int tmp = 0, max = 0, min = 0xFFFF;
	vcc = GetVccVoltage(); // 保留1位小数
	for (i = 0; i < 34; i++)
	{
		tmp = ReadADCNum(vcc, 0); // 读取8次数据
		if (tmp > max)
		{
			max = tmp;
		}
		if (tmp < min)
		{
			min = tmp;
		}
		res += tmp;
	}
	res -= (min + max);
	return (int)(res >> 5);
}
// 返回单位是 mV
int ReadVinADCDat()
{
	unsigned int vcc = 0;
	unsigned int tmp = 0;
	vcc = GetVccVoltage();
	tmp = ReadADCNum(vcc, 1);
	tmp = tmp / 10 * 57; // 公式：5.7 = R22 / (R21 + R22)，实际6.5，除于一个数等于乘于一个数的倒数
	return tmp;
}
unsigned long ReadADCNum(unsigned int vcc, unsigned char ch)
{
	unsigned long dat = 0;
	// Delay10ms();
	// ADC_CONTR &= 0xF0;                      //选择ADC0，引脚P10，单片机：STC8G2K64S4
	ADC_CONTR &= (0xF8 + ch); // 选择ADC8，引脚P00，单片机：STC8G2K64S4 //T12定义
	ADC_CONTR |= 0x40;		  // 启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20))
		;															// 查询ADC完成标志
	ADC_CONTR &= ~0x20;												// 清完成标志
	dat = ((ADC_RES << 8) + ADC_RESL);								// 读取ADC结果
	dat = (((unsigned long)ADC_RES << 8) + ADC_RESL) * vcc / 1024L; // 读取ADC结果 此处单位是:mV; 实际公式：ADC结果 * VCC / 1024
	return dat;
}

unsigned int ADCRead()
{
	unsigned int res;

	ADC_CONTR |= 0x40; // 启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20))
		;							 // 查询ADC完成标志
	ADC_CONTR &= ~0x20;				 // 清完成标志
	res = (ADC_RES << 8) | ADC_RESL; // 读取ADC结果
	return res;
}

unsigned int GetVccVoltage()
{
	int *BGV; // 内部1.19V参考信号源值存放在idata中
	unsigned char i = 0;
	unsigned res = 0, vcc = 0;
	BGV = (int idata *)0xEF;
	ADC_CONTR |= 0x0F; // 选择ADC15,内部1.19V
	for (i = 0; i < 8; i++)
	{
		res += ADCRead(); // 读取8次数据
	}
	res >>= 3;						 // 取平均值
	vcc = (int)(1024L * *BGV / res); //(10位ADC算法)计算VREF管脚电压,即电池电压 单位：mV
	return vcc;
}
