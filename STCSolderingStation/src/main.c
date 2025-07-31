#include "config.h"
#include "arduboy.h"
#include "uart.h"
#include "oled.h"
#include "delay.h"
#include "oledfont.h"
#include "adc.h"
#include "Timer0.h"
#include "EC11.h"
#include "pid_v1.h"
#include "e2prom.h"
#include "tip.h"
#include "string.h"

#include "sysconst.h"

#define EX2 0x10

int Setpoint = 0; // 设定温度
// int TempInOLED = -1;
int CurrentTemp = 0; // 测得烙铁温度
unsigned char NumberOfTips = 1;
int tempRaw = 0, tempTmpRaw = 0; // 烙铁ADC值

// unsigned char voltage = 0;
unsigned char pwm = 0;
int CurrentTip = 0;
int ShowTemp = 0;
int SetTemp = 0;
// State variables
bit inSleepMode = 0;
bit inOffMode = 0;
bit inBoostMode = 0;
bit inCalibMode = 0;
bit isWorky = 1;
bit beepIfWorky = 1;
bit TipIsPresent = 1;
bit PIDenable = PID_ENABLE;
bit beepEnable = BEEP_ENABLE;
bit BUTTON = 0;
bit IS_LONGPRESS = 0;
bit handleMoved = 0;
bit rotaryMoved = 0;
bit PowerOnHeatEnable = POWER_ON_HEAT_ENABLE;
bit d0 = 1;
unsigned char boost_s = 0;
unsigned char status = 0;
unsigned char goneMinutes = 0;
unsigned long boostmillis;
unsigned long sleepmillis;
unsigned long goneSeconds4sleep;
volatile int count, countMin, countMax, countStep;

int pdata aggKp = 11, aggKi = 5, aggKd = 1;
int pdata consKp = 11, consKi = 3, consKd = 5;

// Menu Item
char code *xdata SetupItems[] = {"Setup Menu", "Tip Settings", "Temp Settings", "Timer Settings", "Control Type", "Main Screen", "Buzzer", "Power On Heat", "Information", "Return"};
char code *xdata TipItems[] = {"Tip:", "Change Tip", "Calibrate Tip", "Rename Tip", "Delete Tip", "Add new Tip", "Calibration", "Return"};
char code *xdata StoreItems[] = {"Store Settings ?", "No", "Yes"};
char code *xdata MainScreenItems[] = {"Main Screen", "Big Numbers", "More Infos"};
char code *xdata TimerItems[] = {"Timer Settings", "Sleep Timer", "QK Sleep Timer", "Off Timer", "Boost Timer", "Return"};
char code *xdata MaxTipMessage[] = {"Warning", "You reached", "maximum number", "of tips!"};
char code *xdata DeleteMessage[] = {"Warning", "You cannot", "delete your", "last tip!"};
char code *xdata SureItems[] = {"Are you sure ?", "No", "Yes"};
char code *xdata DefaultTempItems[] = {"Default Temp", "deg C"};
char code *xdata SleepTempItems[] = {"Sleep Temp", "deg C"};
char code *xdata BoostTempItems[] = {"Boost Temp", "deg C"};
char code *xdata SleepTimerItems[] = {"Sleep Timer", "Minutes"};
char code *xdata QuickSleepTimerItems[] = {"Quick Sleep", "Seconds"};
char code *xdata OffTimerItems[] = {"Off Timer", "Minutes"};
char code *xdata BoostTimerItems[] = {"Boost Timer", "Seconds"};
char code *xdata TempItems[] = {"Temp Settings", "Default Temp", "Sleep Temp", "Boost Temp", "Return"};
char code *xdata ControlTypeItems[] = {"Control Type", "Direct", "PID"};
char code *xdata BuzzerItems[] = {"Buzzer", "Disable", "Enable"};
char code *xdata PowerOnHeatItems[] = {"Power On Heat", "Disable", "Enable"};

void SetupScreen();
unsigned char MenuScreen(char code *xdata Items[], unsigned char numberOfItems, unsigned char selected);
void MessageScreen(char code *xdata Items[], unsigned char numberOfItems);
unsigned char ReadBtn();
void InitIOModel();
int constrain(int value, int min, int max);
void Show2OLED(char *str, unsigned char lineno, unsigned char columnno);
void Beep(unsigned char cnt);
void MainScreen();
int Map(int x, int in_min, int in_max, int out_min, int out_max);
void ResetADC();
void PWM2Tips(unsigned long rate);
int calculateTemp(int rawAdc);
int getRotary();
void setRotary(int rmin, int rmax, unsigned char rstep, int rvalue);
void ROTARYCheck();
void SENSORCheck();
void SLEEPCheck();
void Thermostat();
void UpdateEEPROM();
void GetEEPROM();

unsigned int pdata DefaultTemp = TEMP_DEFAULT;
unsigned int pdata SleepTemp = TEMP_SLEEP;
unsigned char pdata BoostTemp = TEMP_BOOST;
unsigned char pdata time2sleep = TIME2SLEEP;
unsigned char pdata time2off = TIME2OFF;
unsigned char pdata timeOfBoost = TIMEOFBOOST;
unsigned char pdata time2quickSleep = TIMEOFBOOST;
unsigned char pdata MainScrType = MAINSCREEN;

// unsigned int code CalTemp[] = {418,823,1242,1650,1851};
int pdata CalTemp[TIPMAX][4] = {TEMP200, TEMP280, TEMP360, TEMPCHP};
char xdata TipName[TIPMAX][TIPNAMELENGTH] = {TIPNAME};

PidType *ppt;

void main()
{
    PidType pt = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    ppt = &pt;

    InitIOModel();
    EA = 1;
    ES = 1;
    // Init
    UartInit();
    InitArduboy();
    OLED_Init();
    InitADC();
    INTCLKO = EX2;
    InitTimer0();
    InitEc11();

    Beep(1);
    WORKLED = 0;

    ResetADC();

    PID_init(ppt, aggKp, aggKi, aggKd, PID_Direction_Direct);
    PID_SetMode(ppt, PID_Mode_Automatic);
    pt.myInput = CurrentTemp;
    PID_Compute(ppt);

    UartSendStr(" ", 0);
    GetEEPROM();

    countMin = 50;
    countMax = 400;
    countStep = 10;
    sleepmillis = (unsigned int)time2sleep * 60;

    Setpoint = DefaultTemp;
    setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, DefaultTemp);
    if (!PowerOnHeatEnable)
    {
        // 上电后处于自动关机状态（不加热）
        inSleepMode = 1;
        inOffMode = 1;
    }
    while (1)
    {
        /***** 正式代码Begin *****/
        // 获取芯片电压
        // voltage = (unsigned char)(GetVccVoltage() / 100);
        // voltage = (unsigned char)(ReadVinADCDat() / 100);

        // printf("%d",(int)sizeof(p));
        ROTARYCheck();
        SLEEPCheck();
        Thermostat();
        MainScreen();
        /***** 正式代码End *****/
    }
}
void Thermostat()
{
    if (SWPIN != d0)
    {
        handleMoved = 1;
        d0 = SWPIN;
    }

    PWM2Tips(0);
    Delay1ms();
    tempTmpRaw = ReadADCDat();
    // 按比例累积烙铁头ADC值，消除ADC噪音
    tempRaw += ((tempTmpRaw - tempRaw) * 5 / 100);
    CurrentTemp = calculateTemp(tempRaw);

    if ((ShowTemp != Setpoint) || (abs(ShowTemp - CurrentTemp) > 5))
        ShowTemp = CurrentTemp;
    if (abs(ShowTemp - Setpoint) <= 1)
        ShowTemp = Setpoint;
    if (abs(SetTemp - CurrentTemp) < 5)
    {
        if (!isWorky && beepIfWorky)
            Beep(1);
        isWorky = 1;
        beepIfWorky = 0;
    }
    else
    {
        isWorky = 0;
    }
    if (ShowTemp > 500)
        TipIsPresent = 0;                  // tip removed ?
    if (!TipIsPresent && (ShowTemp < 500)) // new tip inserted ?
    {
        PWM2Tips(0);                                        // shut off heater
        Beep(1);                                            // beep for info
        TipIsPresent = 1;                                   // tip is present now
        ChangeTipScreen();                                  // show tip selection screen
        updateEEPROM();                                     // update setting in EEPROM
        handleMoved = 1;                                    // reset all timers
        ResetADC();                                         // restart temp smooth algorithm
        setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, Setpoint); // reset rotary encoder
    }

    if (inOffMode)
        Setpoint = 0;
    else if (inSleepMode)
        Setpoint = SleepTemp;
    else if (inBoostMode)
        Setpoint = SetTemp + BoostTemp;
    else
        Setpoint = SetTemp;

    if (PIDenable)
    {
        if (abs(Setpoint - CurrentTemp) < 30)
            PID_SetTunings(ppt, consKp, consKi, consKd);
        else
            PID_SetTunings(ppt, aggKp, aggKi, aggKd);
        ppt->mySetpoint = Setpoint;
        ppt->myInput = CurrentTemp;
        PID_Compute(ppt); // PID计算
        pwm = ppt->myOutput;
        if (pwm > 70)
            status = 0x01;
        else
            status = 0x06;
    }
    else
    {
        if (CurrentTemp + 1 < Setpoint)
            pwm = 100;
        else
            pwm = 0;
    }
    PWM2Tips(pwm); // 设置加热功率(PWM)
}

void ROTARYCheck()
{
    unsigned char btn = ReadBtn();
    SetTemp = getRotary();

    if (btn)
    {
        handleMoved = 1;
        rotaryMoved = 1;
        if (btn > 1)
        {
            SLEEPCheck();
            SetupScreen();
            BUTTON = 0;
        }
        else if (btn > 0)
        {
            if (!inOffMode && !inSleepMode)
            {
                Beep(1);
                inBoostMode = !inBoostMode;
                if (inBoostMode)
                {
                    boostmillis = millis();
                }
            }
        }
    }
    if (inBoostMode && timeOfBoost)
    {
        if ((millis() - boostmillis) / 1000 >= timeOfBoost)
        {
            inBoostMode = 0;
            Beep(1);
            beepIfWorky = 1;
        }
    }
}
// check and activate/deactivate sleep modes
void SLEEPCheck()
{
    if (handleMoved) // if handle was moved
    {
        if (inOffMode && !rotaryMoved)
        {
            handleMoved = 0;
            return;
        }
        if (inSleepMode) // in sleep or off mode?
        {
            if ((CurrentTemp + 20) < SetTemp) // if temp is well below setpoint
                PWM2Tips(100);                // then start the heater right now
            Beep(1);                          // beep on wake-up
            beepIfWorky = 1;                  // beep again when working temperature is reached
        }
        handleMoved = 0; // reset handleMoved flag
        inSleepMode = 0; // reset sleep flag
        if (rotaryMoved)
        {
            inOffMode = 0;   // reset off flag
            rotaryMoved = 0; // reset rotaryMoved flag
        }
        sleepmillis = millis(); // reset sleep timer
    }

    goneMinutes = (millis() - sleepmillis) / 60000;
    goneSeconds4sleep = (millis() - sleepmillis) / 1000;
    // printf("sleep:%u\n",(unsigned int)(millis() - sleepmillis));
    if ((!inSleepMode) && goneMinutes >= time2sleep && time2sleep > 0)
    {
        inSleepMode = 1;
        Beep(5);
    }
    // quick sleep setting for Accelerometer sensor
    if ((!inSleepMode) && (time2quickSleep > 0) && (goneSeconds4sleep >= time2quickSleep))
    {
        inSleepMode = 1;
        Beep(5);
    }
    if ((!inOffMode) && (time2off > 0) && (goneMinutes >= time2off))
    {
        inOffMode = 1;
        Beep(1);
        Delay150ms();
        Delay150ms();
        Beep(5);
    }
    // 关机提醒
    if (inOffMode && millis() > 1000 && (millis() - sleepmillis) / 60000 % 5 == 0 // 一分钟的时候
        && ((millis() - sleepmillis) % 60000) < 300)
    {
        Beep(2);
        Delay150ms();
        Beep(2);
        Delay150ms();
        Beep(2);
    }
}

void MainScreen()
{
    unsigned char buffer[10] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00};
    unsigned char voltage = (unsigned char)(ReadVinADCDat() / 100);
    Clear();
    sprintf(buffer, "SET:%d", Setpoint);
    buffer[9] = 0x00;
    Print(0, 0, buffer, 2);

    // status
    if (CurrentTemp > 500 && millis() / 1000 % 2 == 0)
        sprintf(buffer, "%5s", "ERROR");
    else if (inOffMode)
        sprintf(buffer, "%5s", "OFF");
    else if (inSleepMode)
        sprintf(buffer, "%5s", "SLEEP");
    else if (inBoostMode)
        sprintf(buffer, "%5s", "BOOST");
    else if (isWorky)
        sprintf(buffer, "%5s", "WORK");
    else if (pwm > 70)
        sprintf(buffer, "%5s", "HEAT");
    else
        sprintf(buffer, "%5s", "HOLD");

    Print(88, 0, buffer, 2);

    if (MainScrType)
    {
        // temp
        if (ShowTemp > INVALID_TEMP)
        {
            sprintf(buffer, "000", ShowTemp);
        }
        else
        {
            sprintf(buffer, "%d", ShowTemp);
        }
        if (ShowTemp > 99)
        {
            Print(13, 20, buffer, 3);
        }
        else
        {
            Print(23, 20, buffer, 3);
        }

        // tempRaw
        //		sprintf(buffer,"RAW:%3d",tempRaw);
        //		Print(0,20,buffer,1);

        // PWM
        sprintf(buffer, "%3d%%", (int)pwm);
        Print(88, 33, buffer, 2);

        // Tips
        Print(0, 50, TipName[CurrentTip], 2);

        if (handleMoved)
            Print(50, 52, " *", 2);
        else
            Print(50, 52, "* ", 2);

        // Voltage
        sprintf(buffer, " %2d.%dV", (int)voltage / 10, (int)voltage % 10);
        Print(75, 50, buffer, 2);
    }
    else
    {
        // temp
        if (ShowTemp > INVALID_TEMP)
        {
            sprintf(buffer, "000", ShowTemp);
        }
        else
        {
            sprintf(buffer, "%d", ShowTemp);
        }
        if (ShowTemp > 99)
        {
            Print(0, 20, buffer, 4);
        }
        else
        {
            Print(16, 20, buffer, 4);
        }
        // PWM
        sprintf(buffer, "%3d%%", (int)pwm);
        Print(90, 50, buffer, 2);
    }

    OLED_WR_Frame(sBuffer);
}

// setup screen
void SetupScreen()
{
    unsigned int SaveSetTemp = SetTemp;
    unsigned char selection = 0;
    bit repeat = 1;
    PWM2Tips(0); // 关闭加热
    Beep(1);

    while (repeat)
    {
        selection = MenuScreen(SetupItems, sizeof(SetupItems), selection);
        switch (selection)
        {
        case 0:
            TipScreen();
            repeat = 0;
            break;
        case 1:
            TempScreen();
            break;
        case 2:
            TimerScreen();
            break;
        case 3:
            PIDenable = MenuScreen(ControlTypeItems, sizeof(ControlTypeItems), PIDenable);
            break;
        case 4:
            MainScrType = MenuScreen(MainScreenItems, sizeof(MainScreenItems), MainScrType);
            break;
        case 5:
            beepEnable = MenuScreen(BuzzerItems, sizeof(BuzzerItems), beepEnable);
            break;
        case 6:
            PowerOnHeatEnable = MenuScreen(PowerOnHeatItems, sizeof(PowerOnHeatItems), PowerOnHeatEnable);
            break;
        case 7:
            InfoScreen();
            break;
        default:
            repeat = 0;
            break;
        }
    }
    Clear();
    Print(28, 28, "Saving...", 2);
    OLED_WR_Frame(sBuffer);
    Delay300ms();
    UpdateEEPROM();
    handleMoved = 1;
    SetTemp = SaveSetTemp;
    setRotary(TEMP_MIN, TEMP_MAX, TEMP_STEP, Setpoint);
}

// menu screen
unsigned char MenuScreen(char code *xdata Items[], unsigned char numberOfItems, unsigned char selected)
{
    bit isTipScreen, lastbutton;
    unsigned char pdata lastselected = selected, i, drawnumber;
    char pdata arrow = 0;
    if (selected)
        arrow = 1;
    numberOfItems >>= 1;
    setRotary(0, numberOfItems - 2, 1, selected);
    lastbutton = !ReadBtn(); //;Print(35,20,buffer,3);
    isTipScreen = !strcmp(Items[0], "Tip:");
    // Clear();

    do
    {
        selected = (unsigned char)getRotary();
        // printf("Menu:%d\n",(int)selected);

        arrow = constrain(arrow + selected - lastselected, 0, 2);
        lastselected = selected;
        Print(0, 0, Items[0], 2);
        if (isTipScreen)
        {
            Print(54, 0, TipName[CurrentTip], 2);
        }
        Print(0, 16 * (arrow + 1), ">", 2);
        for (i = 0; i < 3; i++)
        {
            drawnumber = selected + i + 1 - arrow;
            if (drawnumber < numberOfItems)
            {
                Print(12, 16 * (i + 1), Items[selected + i + 1 - arrow], 2);
                // printf(Items[selected + i + 1 - arrow]);
            }
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
    return selected;
}
void MessageScreen(char code *xdata Items[], unsigned char numberOfItems)
{
    unsigned char pdata i;
    bit lastbutton = !ReadBtn();
    Clear();
    for (i = 0; i < numberOfItems; i++)
        Print(0, i * 16, Items[i], 2);

    OLED_WR_Frame(sBuffer);
    do
    {
        if (lastbutton && ReadBtn())
        {
            Delay10ms();
            lastbutton = 0;
        }
    } while (ReadBtn() || lastbutton);
    Beep(1);
}

// information display screen
void InfoScreen()
{
    char pdata version;
    unsigned char pdata vin = 0;
    unsigned char pdata chipVol = 0;
    char pdata buffer[15];
    bit lastbutton = !ReadBtn();
    version = VERSION;

    do
    {
        vin = (unsigned char)(ReadVinADCDat() / 100);
        chipVol = (unsigned char)(GetVccVoltage() / 100);
        sprintf(buffer, "Firmware: v%d.%d", (int)(version / 10), (int)(version % 10));
        Print(0, 0, buffer, 2);

        Print(0, 16, "Tmp: N/A", 2);
        sprintf(buffer, "Vin: %d.%d V", (int)vin / 10, (int)vin % 10);
        Print(0, 32, buffer, 2);
        sprintf(buffer, "Vcc: %d.%d V", (int)chipVol / 10, (int)chipVol % 10);
        Print(0, 48, buffer, 2);
        OLED_WR_Frame(sBuffer);
        Clear();

        if (lastbutton && ReadBtn())
        {
            Delay10ms();
            lastbutton = 0;
        }
    } while (ReadBtn() || lastbutton);
    Beep(1);
}

void ResetADC()
{
    unsigned char i = 0;
    unsigned long tmp = 0;
    for (i = 0; i < 32; i++)
    {
        tmp += ReadADCDat(); // 初始化温度 ADC值
    }
    tempRaw = tmp >> 5;
    // printf("*dat:%d",*dat);
}

int calculateTemp(int rawAdc)
{
    if (rawAdc < 418)
        return Map(rawAdc, 0, 418, 21, 110);
    else if (rawAdc < 823)
        return Map(rawAdc, 418, 823, 111, CalTemp[CurrentTip][0]);
    else if (rawAdc < 1242)
        return Map(rawAdc, 823, 1242, CalTemp[CurrentTip][0], CalTemp[CurrentTip][1]);
    else
        return Map(rawAdc, 1242, 1650, CalTemp[CurrentTip][1], CalTemp[CurrentTip][2]);
}

// 等比映射函数
int Map(int x, int in_min, int in_max, int out_min, int out_max)
{
    // printf("outmax:%d\t",out_max);
    // printf("left:%u ",out_max >> 8);
    // printf("right:%u\n",out_max & 0xFF);
    return (int)((long)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void Beep(unsigned char cnt)
{
    if (beepEnable)
    {
        CCON = 0x00;
        CMOD = 0x0E;
        CL = 0;
        CH = 0;
        PCA_PWM1 = 0xE8;
        // PCA_PWM1 = PCA_PWM1 | 0x28;
        CCAPM1 = 0x42;
        CCAP1H = 0x00;
        CCAP1L = 0x00;
        CR = 1;
        cnt *= 10;
        while (cnt--)
        {
            Delay10ms();
        }
        PCA_PWM1 = 0x00;
        CCAPM1 = 0x00;
        CR = 0;
    }
}

void PWM2Tips(unsigned long rate)
{
    //	printf("rate:%d%% ",(int)rate);
    if (rate == 0)
    {
        PCA_PWM0 = 0xC0;
        CCAP0L = 0x00;
        CCAP0H = 0x00;
        CR = 0;
        return;
    }
    if (rate >= 100)
    {
        rate = 0x7FF;
    }
    else
    {
        rate = 1024 * rate / 100;
    }

    CCON = 0x00;
    CMOD = 0x00; // PCA时钟为系统时钟
    CL = 0x00;
    CH = 0x00;

    CCAPM0 = 0x42;   // PCA模块0为PWM工作模式
    PCA_PWM0 = 0xC0; // PCA模块0输出10位PWM

    /* 重要提示，PCA_PWM0寄存器只能直接使用赋值运算符操作 */
    PCA_PWM0 = (PCA_PWM0 & 0xC0) | (rate >> 4 & 0x30) | (rate >> 6 & 0x0C) | (rate >> 9 & 0x02); // 设置10bit高位 自动重载寄存器 EPCnH 赋值(100%占空比时候需要赋值为1)
    CCAP0H = rate & 0xFF;
    CCAP0L = rate & 0xFF;
    CR = 1;
}

void setRotary(int rmin, int rmax, unsigned char rstep, int rvalue)
{
    countMin = rmin;
    countMax = rmax;
    countStep = rstep;
    count = rvalue;
}

// reads current rotary encoder value
int getRotary()
{
    return count;
}

/*  Ec11调用通知编码器的值 Begin */
void Ec11Proc(char v)
{
    if (v >= 0)
    {
        count += countStep;
    }
    else
    {
        count -= countStep;
    }
    count = constrain(count, countMin, countMax);
    if (!handleMoved || !rotaryMoved)
    {
        handleMoved = 1;
        rotaryMoved = 1; // rotary was rotated
                         // SLEEPCheck();        // check and activate/deactivate sleep modes
    }
}
unsigned char ReadBtn()
{
    if (BUTTON)
    {
        BUTTON = 0;
        if (IS_LONGPRESS)
        {
            IS_LONGPRESS = 0;
            return 2;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}
void Int2Isr() interrupt 10
{
    char timeout = 70;
    while (!P36 && timeout)
    {
        Delay10ms();
        timeout--;
    }
    if (timeout >= 60)
        return;
    BUTTON = 1;
    if (timeout <= 20)
        IS_LONGPRESS = 1; // 长按
    else
        IS_LONGPRESS = 0;
    //	if(P36 == 0)
    //	{
    //		//UartSend(STATUS_MAC);
    //	}
}
/*  Ec11调用通知编码器的值 End */

int constrain(int value, int min, int max)
{
    if (value < min)
    {
        return min;
    }
    if (value > max)
    {
        return max;
    }
    return value;
}
void GetEEPROM()
{
    unsigned char i, j;
    unsigned int counter = 17;
    unsigned int identifier = (unsigned int)Read(0) << 8 | Read(1);
    if (identifier == EEPROM_IDENT)
    {
        DefaultTemp = (unsigned int)Read(2) << 8 | (unsigned char)Read(3);
        SleepTemp = (unsigned int)Read(4) << 8 | (unsigned char)Read(5);
        BoostTemp = Read(6);

        time2sleep = (unsigned char)Read(7);
        time2off = Read(8);
        timeOfBoost = Read(9);
        MainScrType = Read(10);
        PIDenable = Read(11);
        beepEnable = Read(12);
        CurrentTip = Read(13);

        NumberOfTips = Read(14);
        time2quickSleep = Read(15);
        PowerOnHeatEnable = Read(16);

        for (i = 0; i < NumberOfTips; i++)
        {
            for (j = 0; j < TIPNAMELENGTH; j++)
            {
                TipName[i][j] = Read(counter++);
            }
            for (j = 0; j < 4; j++)
            {
                CalTemp[i][j] = Read(counter++) << 8;
                CalTemp[i][j] |= (unsigned char)Read(counter++);
            }
        }
    }
    else
    {
        UpdateEEPROM();
    }
}
void UpdateEEPROM()
{
    unsigned char i, j;
    unsigned int counter = 17;
    IapErase();
    Write(0, EEPROM_IDENT >> 8);
    Write(1, EEPROM_IDENT & 0xFF);
    Write(2, DefaultTemp >> 8);
    Write(3, DefaultTemp & 0xFF);
    Write(4, SleepTemp >> 8);
    Write(5, SleepTemp & 0xFF);
    Write(6, BoostTemp);

    Write(7, time2sleep);
    Write(8, time2off);
    Write(9, timeOfBoost);
    Write(10, MainScrType);
    Write(11, PIDenable);
    Write(12, beepEnable);
    Write(13, CurrentTip);

    Write(14, NumberOfTips);
    Write(15, time2quickSleep);
    Write(16, PowerOnHeatEnable);

    // Write(4,S);
    for (i = 0; i < NumberOfTips; i++)
    {
        for (j = 0; j < TIPNAMELENGTH; j++)
            Write(counter++, TipName[i][j]);
        for (j = 0; j < 4; j++)
        {
            Write(counter++, CalTemp[i][j] >> 8);
            Write(counter++, CalTemp[i][j] & 0xFF);
        }
    }
}

void InitIOModel()
{
    P0M0 = 0x00;
    P0M1 = 0xFF;
    P1M0 = 0x03;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0xFF;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;

    P_SW2 |= 0x80; // 开启访问XFR
    // 开启上拉
    P3PU = 0x4C;
    P_SW2 &= ~0x80; // 关闭访问XFR
}

char putchar(char c)
{
    UartSend(c);
    return c;
}