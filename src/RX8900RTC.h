#ifndef RX8900RTC_h
#define RX8900RTC_h
#include <Time.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif

//RX8900 I2C Address
#define RTC_ADDR               0x32
#define RX8900A_ADRS           0x32

//RX8900 Register Addresses
#define SEC_reg                0x00
#define MIN_reg                0x01
#define HOUR_reg               0x02
#define WEEK_reg               0x03
#define DAY_reg                0x04
#define MONTH_reg              0x05
#define YEAR_reg               0x06
#define RAM_reg                0x07
#define MIN_Alarm_reg          0x08
#define HOUR_Alarm_reg         0x09
#define WEEK_DAY_Alarm_reg     0x0A
#define Timer_Counter_0_reg    0x0B
#define Timer_Counter_1_reg    0x0C
#define Extension_Register_reg 0x0D
#define Flag_Register_reg      0x0E
#define Control_Register_reg   0x0F
#define TEMP_reg               0x17
#define Backup_Function_reg    0x18

// week alarm mask
#define NO_WEEK 0x00
#define SUN 0x01   // 0b00000001
#define MON 0x02   // 0b00000010
#define TUE 0x04   // 0b00000100
#define WED 0x08   // 0b00001000
#define THU 0x10   // 0b00010000
#define FRI 0x20   // 0b00100000
#define SAT 0x40   // 0b01000000


typedef enum {
  NO_WEEK_DAY_ALARM = 0x00,
  WEEK_ALARM        = 0x01,
  DAY_ALARM         = 0x02,
} WEEK_DAY_ALARM_TYPES_t;

typedef enum {
  MINUTE_ALARM   = MIN_Alarm_reg,
  HOUR_ALARM     = HOUR_Alarm_reg,
  WEEK_DAY_ALARM = WEEK_DAY_Alarm_reg,
} ALARM_TYPES_t;

typedef enum {
  INTERRUPT_ENABLE  = true,
  INTERRUPT_DISABLE = false,
} INTERRUPT_ENABLE_t;

typedef enum {
  CYCLE_4096HZ = 0b00000000, // 4096Hz
  CYCLE_64HZ   = 0b00000001, // 64Hz
  CYCLE_SECOND = 0b00000010, // second update
  CYCLE_MINUTE = 0b00000011, // minute update
} SOURCE_CLOCK_TYPES_t;


typedef enum {
  UPDATE_SECOND_INT = 0b00000000, // update second interrupt
  UPDATE_MINUTE_INT = 0b00100000, // update minute interrupt
} USEL_t;


typedef struct {
  unsigned char SEC     = 0x00;//0x00-0x59 (bcd)
  unsigned char MIN     = 0x00;//0x00-0x59 (bcd)
  unsigned char HOUR    = 0x00;//0x00-0x23 (bcd)
  unsigned char WEEK    = 0x01;//SUN=0x01,MON=0x02,TUE=0x04,WED=0x08,THU=0x10,FRI=0x20,SAT=0x40
  unsigned char DAY     = 0x01;//0x00-0x28-0x31)
  unsigned char MONTH   = 0x01;//JUN=0x01,FEB=0x02,MAR=0x03,APR=0x04,MAY=0x05,JUN=0x06,JLY=0x07,AUG=0x08,SEP=0x09,OCT=0x10,NOV=0x11,DEC=0x12
  unsigned char YEAR    = 0x00;//0x00-0x99
  unsigned char RAM;
  unsigned char MIN_Alarm;
  unsigned char HOUR_Alarm;
  unsigned char WEEK_DAY_Alarm;
  unsigned char Timer_Counter_0;
  unsigned char Timer_Counter_1;
  unsigned char Extension_Register;
  unsigned char Flag_Register;
  unsigned char Control_Register;
  unsigned char TEMP;
  unsigned char Backup_Function;
} regRX8900_t;


class RX8900RTC {
  public:

    RX8900RTC();
    void init(void);
    static time_t get(void);    //must be static to work with setSyncProvider() in the Time library
    byte set(time_t t);
    static tmElements_t read(void);
    byte write(tmElements_t tm);
    void setFullAlarm(WEEK_DAY_ALARM_TYPES_t wdAlarmType, byte minute, byte hour, byte daydate);
    void resetAlarm();
    void setAlarm(byte minute, byte hour);
    void setAlarm(byte minute);
    void setDayAlarm(byte minute, byte hour, byte daydate);   // set day of a month to daydate.
    void setWeekAlarm(byte minute, byte hour, byte daydate);  // set week alarm mask to daydate such as SUN | SAT.
    void alarmInterrupt(INTERRUPT_ENABLE_t interruptEnabled);
    bool alarmUp(void);  // Returns AF (Alarm Flag) status and reset AF if AF is "1".
    void setFixedCycleTimer(int timerCounter, SOURCE_CLOCK_TYPES_t sourceCycle);
    void resetFixedCycleTimer();
    void fixedCycleTimerInterrupt(bool interruptEnabled);
    bool fixedCycleTimerUp(void); // Returns TF (Timer Flag) status and reset TF if TF is "1".
    void setTimeUpdateTimer(USEL_t uTiming);
    void timeUpdateTimerInterrupt(bool interruptEnabled);
    bool timeUpdateTimerUp(void);  // Returns UF (Update Flag) status and reset UF if UF is "1".
    bool IS_VLF(void);   // Returns IS VLF (Voltage Low Flag) status. TRUE: Supply voltage drop less than 1.6V or oscillation stopped.
    bool IS_VDET(void);  // Returns IS VDET (Voltage Detection Flag) FLAG status. TRUE: Supply voltage drop less than 1.95V.
    float temperature(void);

  private:

    void COUNT_START(void);
    void ByteWrite(byte reg, byte data);
    byte ByteRead(byte reg);
    static regRX8900_t RegisterRead(void);
    void RESET(void);
    void SET_AIE(void);              //ALARM INTERRUPT ENABLE
    void RESET_AIE(void);            //ALARM INTERRUPT DISABLE
    void RESET_AF(void);             //RESET ALARM FLAG
    byte IS_AF(void);                //IS ALARM TIME?
    void SET_AE(ALARM_TYPES_t reg);  //ALARM ENABLE
    void RESET_AE(ALARM_TYPES_t reg);//ALARM DISABLE

    void SET_TIE(void);              //TIMER INTERRUPT ENABLE
    void RESET_TIE(void);            //TIMER INTERRUPT DISABLE
    void RESET_TF(void);             //RESET TIMER FLAG
    byte IS_TF(void);                //IS TIMER COUNT UP?
    void SET_TE(void);               //TIMER ENABLE
    void RESET_TE(void);             //TIMER DISABLE
    void SET_TSEL_1S(void);          //SET TIMER INTERVAL 1sec
    void SET_TSEL(SOURCE_CLOCK_TYPES_t sourceClock);   //SET FIXED CYCLE INTERRUPT INTERVAL

    void SET_UIE(void);              //UPDATE INTERRUPT ENABLE
    void RESET_UIE(void);            //UPDATE INTERRUPT DISABLE
    void RESET_UF(void);             //RESET UPDATE FLAG
    byte IS_UF(void);                //IS UPDATE INTERRUPT?
    void SET_USEL(USEL_t ust);       //SET UPDATE INTERRUPT SELECT

    byte subZeller( int y, int m, int d );
    byte dec2bcd(uint8_t n);
    static byte bcd2dec(uint8_t n);
};

//extern RX8900RTC RTC;

#endif
