#include "arduboy.h"
#include "oledfont.h"

// ARDUBOY arduboy;
char xdata sBuffer[1024];

// bool nextFrame()
//{
//	return false;
// }

void InitArduboy()
{
	//	arduboy.nextFrame = &nextFrame;
	Clear();
}

void Clear()
{
	unsigned int i = 0;
	for (i = 0; i < 1024; i++)
	{
		sBuffer[i] = 0x00;
	}
}

void DrawPixel(unsigned char x, unsigned char y, bit color)
{
	unsigned char bitDat, dat;
	unsigned int row_offset;
	if (x < 0 || x > (WIDTH - 1) || y < 0 || y > (HEIGHT - 1))
	{
		return;
	}
	bitDat = 1 << (y & 7);
	row_offset = (y & 0xF8) * WIDTH / 8 + x;
	dat = sBuffer[row_offset] | bitDat;
	if (!color)
		dat ^= bitDat;
	sBuffer[row_offset] = dat;
}

void Print(unsigned char x, unsigned char y, unsigned char *str, unsigned char cSize)
{
	unsigned char i = 0, w = 0, h = 0, moving = 0, lineno = 0, lastLineDat = 0, drawBytes, gap = 0;
	char offset = 0;
	unsigned char *Ascii;
	unsigned int pixelIndex = 0;
	// unsigned int drawBytes = 0;
	switch (cSize)
	{
	case 1:
		w = 6;
		h = 8;
		offset = 32;
		break;
	case 2:
		w = 8;
		h = 16;
		offset = 32;
		break;
	case 3:
		w = 16;
		h = 32;
		offset = 32;
		break;
	case 4:
		w = 26;
		h = 48;
		offset = 32;
		gap = 2;
		break;
	default:
		w = 6;
		h = 8;
		offset = 32;
		break;
	}
	if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
		return;
	while (*str)
	{
		switch (cSize)
		{
		case 1:
			Ascii = asc2_0806[*str - offset];
			break;
		case 2:
			Ascii = asc2_1608[*str - offset];
			break;
		case 3:
			Ascii = asc2_3216[*str - offset];
			break;
		case 4:
			Ascii = number_4826[*str - offset];
			break;
		default:
			Ascii = asc2_0806[*str - offset];
			break;
		}
		moving = y % 8;
		lineno = y / 8;
		drawBytes = w * (h / 8 + (y % 8 + 7) / 8);
		for (i = 0; i < drawBytes; i++)
		{
			if (i >= w && i - w >= 0)
				lastLineDat = Ascii[i - w];
			else
				lastLineDat = 0;
			pixelIndex = x + (i - (w * (i / w))) + ((i / w + lineno) * 128);
			if (pixelIndex > 1023)
				break;
			if ((y % 8 + 7) / 8 > 0 && drawBytes - i <= w)
			{
				sBuffer[pixelIndex] = (unsigned char)(lastLineDat >> (8 - moving));
			}
			else
			{
				sBuffer[pixelIndex] = (Ascii[i] << moving) + (unsigned char)(lastLineDat >> (8 - moving));
				// sBuffer[pixelIndex] = (byte)((number_4826[str[index] - 48][i] << moving) + (byte)(lastLineDat >> (8 - moving)));
			}
		}
		str++;
		x += w + gap;
		// y = 0;
	}
}

// void DrawSlowXYBitmap(unsigned char x, unsigned char y, unsigned char *bitmap, unsigned char w, unsigned char h, bit color)
//{
//	unsigned int xi, yi, byteWidth;
//	// no need to draw at all of we're offscreen
//	if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
//		return;

//	xi, yi, byteWidth = (w + 7) / 8;
//	for(yi = 0; yi < h; yi++)
//	{
//		for(xi = 0; xi < w; xi++ )
//		{
//			if(*(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7)))
//			{
//				DrawPixel(x + xi, y + yi, color);
//			}
//		}
//	}
//}
// void FillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, bit color)
//{
//	unsigned char i = 0;
//	// stupidest version - update in subclasses if desired!
//	for (i=x; i<x+w; i++)
//	{
//		DrawFastVLine(i, y, h, color);
//	}
//}
// void DrawFastVLine(unsigned char x, unsigned char y, unsigned char h, bit color)
//{
//	int a,end = y+h;
//	for (a = max(0,y); a < min(end,HEIGHT); a++)
//	{
//		DrawPixel(x,a,color);
//	}
//}
// int min(int val1,int val2)
//{
//	return val1 > val2 ? val2 : val1;
//}
// int max(int val1,int val2)
//{
//	return val1 > val2 ? val1 : val2;
//}