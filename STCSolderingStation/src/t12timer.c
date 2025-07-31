#include "t12timer.h"

// timer settings screen
void TimerScreen()
{
	unsigned char selection = 0;
	bit repeat = 1;
	while (repeat)
	{
		selection = MenuScreen(TimerItems, sizeof(TimerItems), selection);
		switch (selection)
		{
		case 0:
			setRotary(0, 30, 1, time2sleep);
			time2sleep = InputScreen(SleepTimerItems);
			break;
		case 1:
			setRotary(0, 60, 1, time2quickSleep);
			time2quickSleep = InputScreen(QuickSleepTimerItems);
			break;
		case 2:
			setRotary(0, 60, 1, time2off);
			time2off = InputScreen(OffTimerItems);
			break;
		case 3:
			setRotary(0, 180, 10, timeOfBoost);
			timeOfBoost = InputScreen(BoostTimerItems);
			break;
		default:
			repeat = 0;
			break;
		}
	}
}