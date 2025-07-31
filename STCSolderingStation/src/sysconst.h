#ifndef _SYSCONST_H_
#define _SYSCONST_H_

#define TIPMAX 8        // 最大储存烙铁数
#define TIPNAMELENGTH 6 // 烙铁名称长度（5位，最后是\0）

#define TEMP_MIN 150     // min selectable Setpoint
#define TEMP_MAX 400     // max selectable Setpoint
#define TEMP_DEFAULT 320 // default start setpoint
#define TEMP_SLEEP 150   // Setpoint in sleep mode
#define TEMP_BOOST 50    // Setpoint increase in boost mode
#define TEMP_STEP 10     // rotary encoder temp change steps
#define TIPNAME "BC1.5"  // default tip name

#define EEPROM_IDENT 0xE76C

#define PID_ENABLE 0  // enable PID control
#define BEEP_ENABLE 1 // enable/disable buzzer
#define MAINSCREEN 1  // type of main screen (0: big numbers; 1: more infos)

// 默认温度转换值
#define TEMP200 210 // Setpoint at ADC = 200
#define TEMP280 310 // Setpoint at ADC = 280
#define TEMP360 410 // Setpoint at ADC = 360
#define TEMPCHP 30  // chip Setpoint while calibration
// unsigned char CurrentTip = 0;

#define TIME2SLEEP 5      // time to enter sleep mode in minutes
#define TIME2OFF 15       // time to shut off heater in minutes
#define TIMEOFBOOST 5     // time to stay in boost mode in seconds
#define TIME2QUICKSLEEP 0 // 快速休眠
#define POWER_ON_HEAT_ENABLE 1

#define VERSION 10; // 版本

#define INVALID_TEMP 500

// Menu Item
extern char code *xdata SetupItems[9];
extern char code *xdata TipItems[8];
extern char code *xdata StoreItems[3];
extern char code *xdata SureItems[3];
extern char code *xdata TimerItems[6];
extern char code *xdata MaxTipMessage[4];
extern char code *xdata DeleteMessage[4];
extern char code *xdata DefaultTempItems[2];
extern char code *xdata SleepTempItems[2];
extern char code *xdata BoostTempItems[2];
extern char code *xdata TempItems[5];
extern char code *xdata SleepTimerItems[2];
extern char code *xdata QuickSleepTimerItems[2];
extern char code *xdata OffTimerItems[2];
extern char code *xdata BoostTimerItems[2];
extern char code *xdata SleepTimerItems[2];
extern char code *xdata QuickSleepTimerItems[2];
extern char code *xdata OffTimerItems[2];
extern char code *xdata BoostTimerItems[2];
extern char code *xdata ControlTypeItems[3];
extern char code *xdata MainScreenItems[3];
extern char code *xdata BuzzerItems[3];

extern bit IS_LONGPRESS;
extern bit IS_LONGPRESS;

extern unsigned char status;
extern unsigned char NumberOfTips;
extern int pdata CalTemp[TIPMAX][4];
extern char xdata TipName[TIPMAX][TIPNAMELENGTH];
extern int Setpoint;
extern int tempRaw;
extern int tempTmpRaw; // 烙铁ADC值
extern int CurrentTip; // 当前烙铁
extern int ShowTemp;
extern int SetTemp;

extern bit inSleepMode;
extern bit inOffMode;
extern bit inBoostMode;
extern bit inCalibMode;
extern bit isWorky;
extern bit beepIfWorky;
extern bit TipIsPresent;
extern bit PIDenable;
extern bit beepEnable;

extern unsigned int pdata DefaultTemp;
extern unsigned int pdata SleepTemp;
extern unsigned char pdata BoostTemp;
extern unsigned char pdata MainScrType;

extern unsigned char pdata time2sleep;
extern unsigned char pdata time2quickSleep;
extern unsigned char pdata time2off;
extern unsigned char pdata timeOfBoost;

unsigned char MenuScreen(char code *xdata Items[], unsigned char numberOfItems, unsigned char selected);
void MessageScreen(char code *Items[], unsigned char numberOfItems);
int getRotary();
void setRotary(int rmin, int rmax, unsigned char rstep, int rvalue);
int constrain(int value, int min, int max);
unsigned char ReadBtn();
void Beep(unsigned char cnt);
void PWM2Tips(unsigned long rate);
void InfoScreen();

#endif