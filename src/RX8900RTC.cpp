 /*-----------------------------------------------------------------------------*
 * RX8900RTC.cpp - Arduino library for the SEIKO EPSON CORPORATION              *
 * RX8900SA CE Real-Time Clock Module.                                          *
 * This library is intended for use with the Arduino Time.h library,            *
 * http://www.arduino.cc/playground/Code/Time                                   *
 *                                                                              *
 * This library is baed on the sample sketch by AKIZUKI DENSHI TSUSHO CO.,LTD.  *
 * http://akizukidenshi.com/catalog/g/gK-13009/                                 *
 * http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip               *
 *                                                                              *
 * Some codes are based on DS3232RTC.CPP by Jack Christensen 06Mar2013          *
 *                                                                              *
 * Citriena  23Mar2019                                                          *
 *------------------------------------------------------------------------------*/ 
#include <RX8900RTC.h>
#include <Wire.h>


/*----------------------------------------------------------------------*
 * Constructor.                                                         *
 *----------------------------------------------------------------------*/
RX8900RTC::RX8900RTC()
{
    Wire.begin();
}

void RX8900RTC::init(void)
{
	delay(1000);  //wait Oscillation start time
    ByteWrite(Extension_Register_reg,0b00001000); //set WEEK ALARM , 1Hz to FOUT
    ByteWrite(Flag_Register_reg,     0b00000000); //reset all flag
    ByteWrite(Control_Register_reg,  0b01000000); //reset all flag. do not write 1 to RESET bit
}

/*----------------------------------------------------------------------*
 * Reads the current time from the RTC and returns it as a time_t       *
 * value.                                                               *
 *----------------------------------------------------------------------*/
time_t RX8900RTC::get()
{
    tmElements_t tm = read();
    return( makeTime(tm) );
}

/*----------------------------------------------------------------------*
 * Sets the RTC to the given time_t value.                              *
 *----------------------------------------------------------------------*/
byte RX8900RTC::set(time_t t)
{
    tmElements_t tm;

    breakTime(t, tm);
    return ( write(tm) );
}

/*----------------------------------------------------------------------*
 * Reads the current time from the RTC and returns it in a tmElements_t *
 * structure.                                                           *
 *----------------------------------------------------------------------*/
tmElements_t RX8900RTC::read(void)
{
	tmElements_t tm;

    regRX8900_t reg = RegisterRead();
    tm.Second = bcd2dec(reg.SEC);
    tm.Minute = bcd2dec(reg.MIN);
    tm.Hour = bcd2dec(reg.HOUR);    //assumes 24hr clock
    tm.Wday = reg.WEEK;
    tm.Day = bcd2dec(reg.DAY);
    tm.Month = bcd2dec(reg.MONTH);
    tm.Year = y2kYearToTm(bcd2dec(reg.YEAR));
    return tm;
}

/*----------------------------------------------------------------------*
 * Sets the RTC's time from a tmElements_t structure.                   *
 *----------------------------------------------------------------------*/
byte RX8900RTC::write(tmElements_t tm)
{
  byte dayOfWeek = subZeller(tm.Year, tm.Month, tm.Day);
  tm.Wday = 1 << (dayOfWeek - 1);
  RESET();
  ByteWrite(SEC_reg,dec2bcd(tm.Second));
  ByteWrite(MIN_reg,dec2bcd(tm.Minute));
  ByteWrite(HOUR_reg,dec2bcd(tm.Hour));
  ByteWrite(WEEK_reg,tm.Wday);
  ByteWrite(DAY_reg,dec2bcd(tm.Day));
  ByteWrite(MONTH_reg,dec2bcd(tm.Month));
  ByteWrite(YEAR_reg,dec2bcd(tmYearToY2k(tm.Year)));
  return 0;
}

void RX8900RTC::setFullAlarm(WEEK_DAY_ALARM_TYPES_t wdAlarmType, byte minutes, byte hours, byte daydate)
{
	RESET_AIE();
	if (minutes < 60) {                 // cause an alarm when the minutes match. 60 > cause no minutes match alarm.
		minutes = dec2bcd(minutes);
	} else {
		minutes = 0b10000000;
	}
	ByteWrite(MIN_Alarm_reg, minutes);

	if (hours < 24) {                  // cause an alarm when the hours match. 24> cause no hours match alarm.
		hours = dec2bcd(hours);
	} else {
		hours = 0b10000000;
	}
	ByteWrite(HOUR_Alarm_reg, hours);
	
	if (wdAlarmType == DAY_ALARM) {
		ByteWrite(Extension_Register_reg, (ByteRead(Extension_Register_reg) | 0b01000000));
		daydate = dec2bcd(daydate);
	} else if (wdAlarmType == WEEK_ALARM){
		ByteWrite(Extension_Register_reg, (ByteRead(Extension_Register_reg) & 0b10111111));
		daydate = dec2bcd(daydate);
	} else {
		daydate = 0b10000000;
	}
	ByteWrite(WEEK_DAY_Alarm_reg, daydate);
	SET_AIE();
	RESET_AF();
}

void RX8900RTC::setAlarm(byte minutes, byte hours)
{
  setFullAlarm(NO_WEEK_DAY_ALARM, minutes, hours, 0);
}

void RX8900RTC::setAlarm(byte minutes)
{
  setFullAlarm(NO_WEEK_DAY_ALARM, minutes, 99, 0);
}

void RX8900RTC::setDayAlarm(byte minutes, byte hours, byte daydate)
{
  setFullAlarm(DAY_ALARM, minutes, hours, daydate);
}

void RX8900RTC::setWeekAlarm(byte minutes, byte hours, byte daydate)
{
  setFullAlarm(WEEK_ALARM, minutes, hours, daydate);
}
/*----------------------------------------------------------------------*
 * Enable or disable an alarm "interrupt" which asserts the INT pin     *
 * on the RTC.                                                          *
 *----------------------------------------------------------------------*/
void RX8900RTC::alarmInterrupt(INTERRUPT_ENABLE_t interruptEnabled)
{
	if (interruptEnabled) {
		SET_AIE();
	} else {
		RESET_AIE();
	}
}

/*----------------------------------------------------------------------*
 * Returns true or false depending on whether the given alarm has been  *
 * triggered, and resets the alarm flag bit.                            *
 *----------------------------------------------------------------------*/
bool RX8900RTC::alarm()
{
	boolean isAF = !(IS_AF() == 0) ;
	RESET_AF();
	return isAF;
}


void RX8900RTC::setCycleTimer(int timerCounter, CLOCK_TYPES_t sClock) {

	RESET_TE();
	RESET_TF();
	RESET_TIE();
	ByteWrite(Timer_Counter_0_reg,timerCounter % 256);
	ByteWrite(Timer_Counter_1_reg,timerCounter / 256);
	SET_TSEL(sClock);
	SET_TE();
	RESET_TF();
}


void RX8900RTC::cylceTimerInterrupt(bool interruptEnabled) {
	if (interruptEnabled) {
		SET_TIE();
	} else {
		RESET_TIE();
	}
}

/*----------------------------------------------------------------------*
 * Returns true or false depending on whether the timer has been count  *
 * upped. Timer flag bit is automatically reset after specific time     *
 * depending on the source clock.                                       *
 *----------------------------------------------------------------------*/
bool RX8900RTC::cycleTimer() {
	bool tf = (IS_TF() != 0);
	RESET_TF();
	return tf;
}


/*----------------------------------------------------------------------*
 * uTiming is 0: Update interrupt timing is once per seconds (default)  *
 * uTiming is 1: Update interrupt timing is once per minutes            *
 *----------------------------------------------------------------------*/
void RX8900RTC::setTimeUpdateTimer(USEL_t uTiming) {
	RESET_UIE();
	SET_USEL(uTiming);
}

void RX8900RTC::timeUpdateTimerInterrupt(bool interruptEnabled) {
	if (interruptEnabled) {
		SET_UIE();
	} else {
		RESET_UIE();
	}
}

/*----------------------------------------------------------------------*
 * Returns true or false depending on whether the update event has been *
 * occured. Update flag bit is automatically reset after specific time  *
 * depending on the USEL.                                               *
 *----------------------------------------------------------------------*/
bool RX8900RTC::timeUpdateTimer() {
	bool uf = (IS_UF() != 0);
	RESET_UF();
	return uf;
}

/*----------------------------------------------------------------------*
 * Returns the temperature in Celsius.                                  *
 *----------------------------------------------------------------------*/
float RX8900RTC::temperature(void)
{
    return (ByteRead(TEMP_reg) * 2 - 187.19) / 3.218;
}


//*******************************************************
//--------------------------------------------------------
void RX8900RTC::COUNT_START(void)
{  
  ByteWrite(SEC_reg,0x00);
  RESET();
}
//--------------------------------------------------------
void RX8900RTC::ByteWrite(byte reg, byte data)
{
  Wire.beginTransmission(RX8900A_ADRS);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

byte RX8900RTC::ByteRead(byte reg)
{
  byte data = 0;
  Wire.beginTransmission(RX8900A_ADRS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(RX8900A_ADRS, 1);
  data = Wire.read();                     //RECEIVE 1BYTE
  return data;
}

regRX8900_t RX8900RTC::RegisterRead(void)
{
  regRX8900_t regs;
  Wire.beginTransmission(RX8900A_ADRS);
  Wire.write(SEC_reg);                   //set 0x00(SEC)Register address
  Wire.endTransmission(false);
  Wire.requestFrom(RX8900A_ADRS, Control_Register_reg); //set 0x0F(Control Register)Register address
  regs.SEC = Wire.read();                //0x00
  regs.MIN = Wire.read();                //0x01
  regs.HOUR = Wire.read();               //0x02
  regs.WEEK = Wire.read();               //0x03
  regs.DAY = Wire.read();                //0x04
  regs.MONTH = Wire.read();              //0x05
  regs.YEAR = Wire.read();               //0x06
  regs.RAM = Wire.read();                //0x07
  regs.MIN_Alarm = Wire.read();          //0x08
  regs.HOUR_Alarm = Wire.read();         //0x09
  regs.WEEK_DAY_Alarm = Wire.read();     //0x0A
  regs.Timer_Counter_0 = Wire.read();    //0x0B
  regs.Timer_Counter_1 = Wire.read();    //0x0C
  regs.Extension_Register = Wire.read(); //0x0D
  regs.Flag_Register = Wire.read();      //0x0E
  regs.Control_Register = Wire.read();   //0x0F
  Wire.beginTransmission(RX8900A_ADRS);
  Wire.write(TEMP_reg);                  //set TEMP_reg 0x17
  Wire.endTransmission(false);
  Wire.requestFrom(RX8900A_ADRS, 2);     //Backup_Function_reg ~ TEMP_reg
  regs.TEMP = Wire.read();               //0x17
  regs.Backup_Function = Wire.read();    //0x18
  return regs;
}

/*----------------------------------------------------------------------*
 * function for accurate time ajust                                     *
 * reset subsecond counter in Clock&Calendar circuit                    *
 *----------------------------------------------------------------------*/
void RX8900RTC::RESET(void)
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00000001));
}

//---------------------------
void RX8900RTC::SET_AIE(void)           //ALARM INTERRUPT ENABLE
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00001000));
}

//---------------------------
void RX8900RTC::RESET_AIE(void)         //ALARM INTERRUPT DISABLE
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) & 0b11110111));
}

//---------------------------
void RX8900RTC::RESET_AF(void)          //RESET ALARM FLAG
{
  ByteWrite(Flag_Register_reg,(ByteRead(Flag_Register_reg) & 0b11110111));
}

//---------------------------
byte RX8900RTC::IS_AF(void)             //IS ALARM TIME?
{
  return (ByteRead(Flag_Register_reg) & 0b00001000);
}

//---------------------------
void RX8900RTC::SET_AE(ALARM_TYPES_t reg)          //ALARM ENABLE
{
	ByteWrite(reg, ByteRead(reg) & 0b01111111);
}

//---------------------------
void RX8900RTC::RESET_AE(ALARM_TYPES_t reg)          //ALARM DISABLE
{
	ByteWrite(reg, ByteRead(reg) | 0b10000000);
}

//---------------------------
void RX8900RTC::SET_TIE(void)           //TIMER INTERRUPT ENABLE
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00010000));
}

//---------------------------
void RX8900RTC::RESET_TIE(void)         //TIMER INTERRUPT DISABLE
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) & 0b11101111));
}

//---------------------------
void RX8900RTC::RESET_TF(void)          //RESET TIMER FLAG
{
  ByteWrite(Flag_Register_reg,(ByteRead(Flag_Register_reg) & 0b11101111));
}

//---------------------------
byte RX8900RTC::IS_TF(void)             //IS TIMER COUNT UP?
{
  return (ByteRead(Flag_Register_reg) & 0b00010000);
}

//---------------------------
void RX8900RTC::SET_TE(void)            //TIMER ENABLE
{
  ByteWrite(Extension_Register_reg,(ByteRead(Extension_Register_reg) | 0b00010000));
}

//---------------------------
void RX8900RTC::RESET_TE(void)          //TIMER DISABLE
{
  ByteWrite(Extension_Register_reg,(ByteRead(Extension_Register_reg) & 0b11101111));
}

//---------------------------
void RX8900RTC::SET_TSEL_1S(void)       //SET TIMER INTERVAL 1sec
{
  ByteWrite(Extension_Register_reg,((ByteRead(Extension_Register_reg) & 0b11111100)|0b00000010));
}

void RX8900RTC::SET_TSEL(CLOCK_TYPES_t sClock)          //SET TIMER INTERVAL
{
  ByteWrite(Extension_Register_reg,(ByteRead(Extension_Register_reg) & 0b11111100) | sClock); // set Count down cycle (source clock)
}

//---------------------------
void RX8900RTC::SET_UIE(void)           //UPDATE INTERRUPT ENABLE
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00100000));
}

//---------------------------
void RX8900RTC::RESET_UIE(void)         //UPDATE INTERRUPT DISABLE
{
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) & 0b11011111));
}

//---------------------------
void RX8900RTC::RESET_UF(void)          //RESET UPDATE FLAG
{
  ByteWrite(Flag_Register_reg,(ByteRead(Flag_Register_reg) & 0b11011111));
}

//---------------------------
byte RX8900RTC::IS_UF(void)             //IS UPDATE INTERRUPT?
{
  return (ByteRead(Flag_Register_reg) & 0b00100000);
}

//---------------------------
void RX8900RTC::SET_USEL(USEL_t ust)       //SET UPDATE INTERRUPT SELECT
{
  ByteWrite(Extension_Register_reg,((ByteRead(Extension_Register_reg) & 0b11011111)|ust));
}

bool RX8900RTC::IS_VLF(void)
{
  return (ByteRead(Flag_Register_reg) & 0b00000010);
}

bool RX8900RTC::IS_VDET(void)
{
  return (ByteRead(Flag_Register_reg) & 0b00000001);
}

// Zeller’s Congruence; Find the Day for a Date
// 0: Sunday, 6:Saturday
// https://edu.clipper.co.jp/pg-2-47.html
byte RX8900RTC::subZeller( int y, int m, int d )
{
    if( m < 3 ) {
        y--; m += 12;
    }
    return ( y + y/4 - y/100 + y/400 + ( 13*m + 8 )/5 + d )%7;
}

/*----------------------------------------------------------------------*
 * Decimal-to-BCD conversion                                            *
 *----------------------------------------------------------------------*/
byte RX8900RTC::dec2bcd(byte n)
{
    return n + 6 * (n / 10);
}

/*----------------------------------------------------------------------*
 * BCD-to-Decimal conversion                                            *
 *----------------------------------------------------------------------*/
byte __attribute__ ((noinline)) RX8900RTC::bcd2dec(byte n)
{
    return n - 6 * (n >> 4);
}

//RX8900RTC RTC = RX8900RTC();            //instantiate an RTC object
