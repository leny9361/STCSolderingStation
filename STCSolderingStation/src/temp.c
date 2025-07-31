#include "temp.h"

// Setpoint settings screen
void TempScreen()
{
	unsigned char selection = 0;
	bit repeat = 1;
	while (repeat)
	{
		selection = MenuScreen(TempItems, sizeof(TempItems), selection);
		switch (selection)
		{
		case 0:
			// printf("DefaultTemp:%d\n",DefaultTemp);
			setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, DefaultTemp);
			DefaultTemp = InputScreen(DefaultTempItems);
			break;
		case 1:
			// printf("SleepTemp:%d\n",SleepTemp);
			setRotary(20, 200, TEMP_STEP, SleepTemp);
			SleepTemp = InputScreen(SleepTempItems);
			break;
		case 2:
			// printf("BoostTemp:%d\n",BoostTemp);
			setRotary(10, 100, TEMP_STEP, BoostTemp);
			BoostTemp = InputScreen(BoostTempItems);
			break;
		default:
			repeat = 0;
			break;
		}
	}
}

// input value screen
unsigned int InputScreen(char code *xdata Items[])
{
	char buffer[5];
	unsigned int value;
	bit lastbutton = !ReadBtn();
	Clear();
	do
	{
		value = (unsigned int)getRotary();
		Print(0, 0, Items[0], 2);
		Print(0, 32, ">", 2);
		if (value == 0)
		{
			Print(10, 32, "Deactivated", 2);
		}
		else
		{
			sprintf(buffer, "%u ", value);
			Print(10, 32, buffer, 2);
			Print(34 + ((value / 100 ? 1 : 0) * 8), 32, Items[1], 2);
		}
		OLED_WR_Frame(sBuffer);
		Clear();
		if (lastbutton && ReadBtn())
		{
			Delay10ms();
			lastbutton = 0;
		}
	} while (ReadBtn() || lastbutton);

	Beep(1);
	return value;
}