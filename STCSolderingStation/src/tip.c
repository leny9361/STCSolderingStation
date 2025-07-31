#include "tip.h"

// tip settings screen
void TipScreen()
{
	unsigned char selection = 0;
	bit repeat = 1;
	while (repeat)
	{
		selection = MenuScreen(TipItems, sizeof(TipItems), selection);
		switch (selection)
		{
		case 0:
			ChangeTipScreen();
			break;
		case 1:
			CalibrationScreen();
			break;
		case 2:
			InputNameScreen();
			break;
		case 3:
			DeleteTipScreen();
			break;
		case 4:
			AddTipScreen();
			break;
		case 5:
			CalibrationInfo();
			break;
		default:
			repeat = 0;
			break;
		}
	}
}
// change tip screen
void ChangeTipScreen()
{
	unsigned char selected = (unsigned char)CurrentTip;
	unsigned char lastselected = selected;
	unsigned char i, drawnumber;
	bit lastbutton;
	char arrow = 0;
	if (selected)
		arrow = 1;
	setRotary(0, NumberOfTips - 1, 1, selected);
	lastbutton = !ReadBtn();

	do
	{
		selected = getRotary();
		arrow = constrain(arrow + selected - lastselected, 0, 2);
		lastselected = selected;

		Print(0, 0, "Select Tip", 2);
		Print(0, 16 * (arrow + 1), ">", 2);
		for (i = 0; i < 3; i++)
		{
			drawnumber = selected + i - arrow;
			if (drawnumber < NumberOfTips)
				Print(12, 16 * (i + 1), TipName[selected + i - arrow], 2);
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
	CurrentTip = (int)selected;
}
// Setpoint calibration screen
void CalibrationScreen()
{
	unsigned char pdata buffer[10] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00};
	unsigned int pdata CalTempNew[4];
	unsigned char pdata CalStep, i;
	bit lastbutton;
	for (CalStep = 0; CalStep < 3; CalStep++)
	{
		SetTemp = CalTemp[CurrentTip][CalStep];
		setRotary(100, 500, 1, SetTemp);
		beepIfWorky = true;
		lastbutton = !ReadBtn();

		do
		{
			// SENSORCheck();      // reads Setpoint and vibration switch of the iron
			Thermostat(); // heater control
			if (CurrentTemp > 500)
			{
				Clear();
				Print(3, 29, "Please Insert Tip...", 1);
				OLED_WR_Frame(sBuffer);
				Delay300ms();
				Delay300ms();
				return;
			}
			Print(0, 0, "Calibration", 2);
			Print(0, 16, "Step: ", 2);
			sprintf(buffer, "%d", (int)(CalStep + 1));
			Print(43, 16, buffer, 2);
			Print(43 + 8, 16, " of 3", 2);
			if (isWorky)
			{
				Print(0, 32, "Set measured", 2);
				Print(0, 48, "temp: ", 2);
				sprintf(buffer, "%d", (int)getRotary());
				Print(50, 48, buffer, 2);
			}
			else
			{
				Print(0, 32, "ADC:  ", 2);
				sprintf(buffer, "%d", tempRaw);
				Print(50, 32, buffer, 2);
				Print(0, 48, "Please wait...", 2);
			}
			OLED_WR_Frame(sBuffer);
			Clear();
			if (lastbutton && ReadBtn())
			{
				Delay10ms();
				lastbutton = 0;
			}
		} while (ReadBtn() || lastbutton);

		CalTempNew[CalStep] = getRotary();
		Beep(1);
		Delay10ms();
	}

	PWM2Tips(0); // shut off heater
	if ((CalTempNew[0] + 10 < CalTempNew[1]) && (CalTempNew[1] + 10 < CalTempNew[2]))
	{
		if (MenuScreen(StoreItems, sizeof(StoreItems), 0))
		{
			for (i = 0; i < 4; i++)
				CalTemp[CurrentTip][i] = CalTempNew[i];
		}
	}
}
// input tip name screen
void InputNameScreen()
{
	char pdata buffer[5] = {0x20, 0x20, 0x20, 0x20, 0x00};
	unsigned char pdata value, digit, i;
	bit lastbutton;

	for (digit = 0; digit < (TIPNAMELENGTH - 1); digit++)
	{
		lastbutton = (!ReadBtn());
		setRotary(31, 96, 1, 65);
		do
		{
			value = getRotary();
			if (value == 31)
			{
				value = 95;
				setRotary(31, 96, 1, 95);
			}
			if (value == 96)
			{
				value = 32;
				setRotary(31, 96, 1, 32);
			}

			Print(0, 0, "Enter Tip Name", 2);
			Print(9 * digit, 48, "^", 2);
			for (i = 0; i < digit; i++)
			{
				// sprintf(buffer,"%s",TipName[CurrentTip][i]);
				buffer[0] = TipName[CurrentTip][i];
				buffer[1] = 0x00;
				Print(8 * i, 32, buffer, 2);
			}

			buffer[0] = value;
			buffer[1] = 0x00;
			Print(8 * digit, 32, buffer, 2);

			OLED_WR_Frame(sBuffer);
			Clear();
			if (lastbutton && ReadBtn())
			{
				Delay10ms();
				lastbutton = false;
			}
		} while (ReadBtn() || lastbutton);
		TipName[CurrentTip][digit] = value;
		Beep(1);
		Delay10ms();
	}
	TipName[CurrentTip][TIPNAMELENGTH - 1] = 0;
	// return value;
}
// delete tip screen
void DeleteTipScreen()
{
	unsigned char pdata i, j;
	if (NumberOfTips == 1)
	{
		MessageScreen(DeleteMessage, sizeof(DeleteMessage));
	}
	else if (MenuScreen(SureItems, sizeof(SureItems), 0))
	{
		if (CurrentTip == (NumberOfTips - 1))
		{
			CurrentTip--;
		}
		else
		{
			for (i = CurrentTip; i < (NumberOfTips - 1); i++)
			{
				for (j = 0; j < TIPNAMELENGTH; j++)
					TipName[i][j] = TipName[i + 1][j];
				for (j = 0; j < 4; j++)
					CalTemp[i][j] = CalTemp[i + 1][j];
			}
		}
		NumberOfTips--;
	}
}

// add new tip screen
void AddTipScreen()
{
	if (NumberOfTips < TIPMAX)
	{
		CurrentTip = NumberOfTips++;
		InputNameScreen();
		CalTemp[CurrentTip][0] = TEMP200;
		CalTemp[CurrentTip][1] = TEMP280;
		CalTemp[CurrentTip][2] = TEMP360;
		CalTemp[CurrentTip][3] = TEMPCHP;
	}
	else
	{
		MessageScreen(MaxTipMessage, sizeof(MaxTipMessage));
	}
}
void CalibrationInfo()
{
	char buffer[5];
	unsigned char tipSelected = 0;
	bit lastbutton;
	setRotary(0, NumberOfTips - 1, 1, CurrentTip);

	lastbutton = !ReadBtn();
	Clear();
	do
	{
		tipSelected = getRotary();
		printf("tipSelected:%d", tipSelected);

		Print(0, 0, "Tip: ", 2);
		Print(40, 0, TipName[tipSelected], 2);

		sprintf(buffer, "%d", CalTemp[tipSelected][0]);
		Print(10, 16, "TEMP200: ", 2);
		Print(82, 16, buffer, 2);

		sprintf(buffer, "%d", CalTemp[tipSelected][1]);
		Print(10, 16 * 2, "TEMP280: ", 2);
		Print(82, 16 * 2, buffer, 2);

		sprintf(buffer, "%d", CalTemp[tipSelected][2]);
		Print(10, 16 * 3, "TEMP360: ", 2);
		Print(82, 16 * 3, buffer, 2);
		OLED_WR_Frame(sBuffer);
		Clear();

		if (lastbutton && ReadBtn())
		{
			Delay10ms();
			lastbutton = 0;
		}
	} while (ReadBtn() || lastbutton);
	Beep(1);
	// CurrentTip = selected;
}
