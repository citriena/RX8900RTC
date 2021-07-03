 /*-----------------------------------------------------------------------------*
 * RX8900RTC.cpp - Arduino library for the SEIKO EPSON CORPORATION              *
 * RX8900SA CE Real-Time Clock Module.                                          *
 *                                                                              *
 * This library is baed on the sample sketch by AKIZUKI DENSHI TSUSHO CO.,LTD.  *
 * http://akizukidenshi.com/catalog/g/gK-13009/                                 *
 * http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip               *
 *                                                                              *
 *------------------------------------------------------------------------------*/ 
#include <RX8900RTC.h>
#include <Wire.h>


/*----------------------------------------------------------------------*
 * Constructor.                                                         *
 *----------------------------------------------------------------------*/
RX8900RTC::RX8900RTC() {
    Wire.begin();
}


void RX8900RTC::init(void) {
  delay(1000);  //wait Oscillation start time
  ByteWrite(Extension_Register_reg,0b00001000); //set WEEK ALARM , 1Hz to FOUT
  ByteWrite(Flag_Register_reg,     0b00000000); //reset all flag
  ByteWrite(Control_Register_reg,  0b01000000); //reset all flag. do not write 1 to RESET bit
}


void RX8900RTC::begin(void) {
  init();
}


/*----------------------------------------------------------------------*
 * Reads the current time from the RTC and returns it as a time_t       *
 * value.                                                               *
 *----------------------------------------------------------------------*/
time_t RX8900RTC::get() {
  tmElements_t tm = read();
  return makeTime(tm);
}


/*----------------------------------------------------------------------*
 * Sets the RTC to the given time_t value.                              *
 *----------------------------------------------------------------------*/
uint8_t RX8900RTC::set(time_t t) {
  tmElements_t tm;

  breakTime(t, tm);
  return write(tm);
}


/*----------------------------------------------------------------------*
 * Reads the current time from the RTC and returns it in a tmElements_t *
 * structure.                                                           *
 *----------------------------------------------------------------------*/
tmElements_t RX8900RTC::read(void) {
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
uint8_t RX8900RTC::write(tmElements_t tm) {
  uint8_t dayOfWeek = subZeller(tm.Year, tm.Month, tm.Day);
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


void RX8900RTC::setFullAlarm(WEEK_DAY_ALARM_TYPES_t wdAlarmType, uint8_t minute, uint8_t hour, uint8_t daydate) {
  RESET_AIE();
  if (minute < 60) {                 // cause an alarm when the minute match. 60 > cause no minute match alarm.
    minute = dec2bcd(minute);
    ByteWrite(MIN_Alarm_reg, minute);
  } else {
    RESET_AE(MINUTE_ALARM);
  }

  if (hour < 24) {                  // cause an alarm when the hour match. 24> cause no hour match alarm.
    hour = dec2bcd(hour);
    ByteWrite(HOUR_Alarm_reg, hour);
  } else {
    RESET_AE(HOUR_ALARM);
  }

  if (wdAlarmType == DAY_ALARM) {
    SET_WADA_D();
    daydate = dec2bcd(daydate);
    ByteWrite(WEEK_DAY_Alarm_reg, daydate);
  } else if (wdAlarmType == WEEK_ALARM){
    SET_WADA_W();
    daydate = dec2bcd(daydate);
    ByteWrite(WEEK_DAY_Alarm_reg, daydate);
  } else {
    RESET_AE(WEEK_DAY_ALARM);
  }
  RESET_AF();
//  SET_AIE();
}


void RX8900RTC::setAlarm(uint8_t minute, uint8_t hour) {
  setFullAlarm(NO_WEEK_DAY_ALARM, minute, hour, 0);
}


void RX8900RTC::setAlarm(uint8_t minute) {
  setFullAlarm(NO_WEEK_DAY_ALARM, minute, 99, 0);
}


void RX8900RTC::setDayAlarm(uint8_t minute, uint8_t hour, uint8_t daydate) {
  setFullAlarm(DAY_ALARM, minute, hour, daydate);
}


void RX8900RTC::setWeekAlarm(uint8_t minute, uint8_t hour, WEEK_TYPES_t daydate) {
  setFullAlarm(WEEK_ALARM, minute, hour, (uint8_t)daydate);
}


void RX8900RTC::disableAlarm() {
  RESET_AIE();
  RESET_AF();
  SET_WADA_W();
  ByteWrite(WEEK_DAY_Alarm_reg, 0);
  RESET_AF();
}


/*----------------------------------------------------------------------*
 * Enable or disable an alarm "interrupt" which asserts the INT pin     *
 * on the RTC.                                                          *
 *----------------------------------------------------------------------*/
void RX8900RTC::alarmInterrupt(INTERRUPT_CONTROL_t interrupt) {
  if (interrupt) {
    SET_AIE();
  } else {
    RESET_AIE();
  }
}


/*----------------------------------------------------------------------*
 * Returns true or false depending on whether the given alarm has been  *
 * triggered, and resets the alarm flag bit.                            *
 *----------------------------------------------------------------------*/
bool RX8900RTC::alarm() {
  boolean isAF = IS_AF();
//  boolean isAF = !(IS_AF() == 0) ;
  if (isAF) RESET_AF();  //AF (Alarm Flag) is retained until manual reset.
  return isAF;
}


/*----------------------------------------------------------------------*
 * set the preset countdown value (1 to 4095) to timerCounter           *
 * Fixed-cycle interrupt interval = (timerCounter * courceCycle)        *
 * See Application Manual for details.                                  *
 *----------------------------------------------------------------------*/
void RX8900RTC::setFixedCycleTimer(uint16_t timerCounter, SOURCE_CLOCK_TYPES_t sourceClock) {
  RESET_TE();
  RESET_TF();
  RESET_TIE();
  ByteWrite(Timer_Counter_0_reg,timerCounter % 256);
  ByteWrite(Timer_Counter_1_reg,timerCounter / 256);
  SET_TSEL(sourceClock);
  SET_TE();
  RESET_TF();
//  SET_TIE();
 }


void RX8900RTC::disableFixedCycleTimer() {
  RESET_TE();
  RESET_TF();
  RESET_TIE();
}


/*----------------------------------------------------------------------*
 * When fixed cycle Timer interrupt occured, TF (Timer Flag) become "1" *
 * and is retained until reset, but /INT pin status is automatically    *
 * cleared earliest 7.813 ms after the interrupt occurs.                *
 * See Application Manual for details.                                  *
 *----------------------------------------------------------------------*/
void RX8900RTC::fixedCycleTimerInterrupt(INTERRUPT_CONTROL_t interrupt) {
  if (interrupt) {
    SET_TIE();
  } else {
    RESET_TIE();
  }
}


/*----------------------------------------------------------------------*
 * Returns true or false depending on whether the timerCounter has been *
 * count upped, and resets the TF (Timer flag)  if TF is "1"            *
 *----------------------------------------------------------------------*/
bool RX8900RTC::fixedCycleTimer() {
  bool tf = IS_TF();
//  bool tf = (IS_TF() != 0);
  if (tf)  RESET_TF();        //  TF (Timer Flag) is retained until manual reset.
  return tf;
}


/*----------------------------------------------------------------------*
 * usel                                                              *
 *   UPDATE_SECOND_INT: Update interrupt timing is once per second      *
 *   UPDATE_MINUTE_INT: Update interrupt timing is once per minute      *
 *----------------------------------------------------------------------*/
void RX8900RTC::setTimeUpdateTimer(USEL_t usel) {
  RESET_UIE();
  SET_USEL(usel);
}


/*----------------------------------------------------------------------*
 * When time update interrupt occured, UF (Update Flag) become "1" and  *
 * is retained until reset, but /INT pin status is automatically        *
 * cleared earliest 7.813 ms after the interrupt occurs.                *
 *                                                                      *
 * Time update interrupt function cannot be fully stopped.              *
 * If timeUpdateTimerInterrupt() is set disabled, only changing         *
 * /INT pin status is prevented.                                        *
 * See Application Manual for details.                                  *
 *----------------------------------------------------------------------*/
void RX8900RTC::timeUpdateTimerInterrupt(INTERRUPT_CONTROL_t interrupt) {
  if (interrupt) {
    SET_UIE();
  } else {
    RESET_UIE();  // can only stop /INT status change
  }
}


/*----------------------------------------------------------------------*
 * Returns true or false depending on whether the update event has been *
 * occured and resets the UF if UF is "1"                               *
 *----------------------------------------------------------------------*/
bool RX8900RTC::timeUpdateTimer() {
  bool uf = (IS_UF() != 0);
  if (uf) RESET_UF();    //  UF (Update Flag) is retained until reset.
  return uf;
}


/*----------------------------------------------------------------------*
 * Returns the temperature in Celsius.                                  *
 *----------------------------------------------------------------------*/
float RX8900RTC::temperature(void) {
    return (ByteRead(TEMP_reg) * 2 - 187.19) / 3.218;
}


//*******************************************************
//--------------------------------------------------------
void RX8900RTC::COUNT_START(void) {
  ByteWrite(SEC_reg,0x00);
  RESET();
}


void RX8900RTC::ByteWrite(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(RX8900A_ADRS);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}


uint8_t RX8900RTC::ByteRead(uint8_t reg) {
  uint8_t data = 0;
  Wire.beginTransmission(RX8900A_ADRS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(RX8900A_ADRS, 1);
  data = Wire.read();                     //RECEIVE 1BYTE
  return data;
}


regRX8900_t RX8900RTC::RegisterRead(void) {
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
void RX8900RTC::RESET(void) {
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00000001));
}


//---------------------------
void RX8900RTC::SET_AIE(void) {          //ALARM INTERRUPT ENABLE
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00001000));
}


//---------------------------
void RX8900RTC::RESET_AIE(void) {        //ALARM INTERRUPT DISABLE
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) & 0b11110111));
}


//---------------------------
void RX8900RTC::RESET_AF(void) {         //RESET ALARM FLAG
  ByteWrite(Flag_Register_reg,(ByteRead(Flag_Register_reg) & 0b11110111));
}


//---------------------------
uint8_t RX8900RTC::IS_AF(void) {            //IS ALARM TIME?
  return (ByteRead(Flag_Register_reg) & 0b00001000);
}


//---------------------------
void RX8900RTC::SET_AE(ALARM_TYPES_t reg) {         //ALARM ENABLE ()
  ByteWrite(reg, ByteRead(reg) & 0b01111111);
}


//---------------------------
void RX8900RTC::RESET_AE(ALARM_TYPES_t reg) {         //ALARM DISABLE ()
  ByteWrite(reg, ByteRead(reg) | 0b10000000);
}


void RX8900RTC::SET_WADA_W() {         // SET WADA WEEK ALARM
  ByteWrite(Extension_Register_reg, (ByteRead(Extension_Register_reg) & 0b10111111));
}


void RX8900RTC::SET_WADA_D() {         // SET WADA DAY ALARM
  ByteWrite(Extension_Register_reg, (ByteRead(Extension_Register_reg) | 0b01000000));
}


//---------------------------
void RX8900RTC::SET_TIE(void) {          //TIMER INTERRUPT ENABLE
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00010000));
}


//---------------------------
void RX8900RTC::RESET_TIE(void) {        //TIMER INTERRUPT DISABLE
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) & 0b11101111));
}


//---------------------------
void RX8900RTC::RESET_TF(void) {         //RESET TIMER FLAG
  ByteWrite(Flag_Register_reg,(ByteRead(Flag_Register_reg) & 0b11101111));
}


//---------------------------
uint8_t RX8900RTC::IS_TF(void) {            //IS TIMER COUNT UP?
  return (ByteRead(Flag_Register_reg) & 0b00010000);
}


//---------------------------
void RX8900RTC::SET_TE(void) {           //TIMER ENABLE
  ByteWrite(Extension_Register_reg,(ByteRead(Extension_Register_reg) | 0b00010000));
}


//---------------------------
void RX8900RTC::RESET_TE(void) {         //TIMER DISABLE
  ByteWrite(Extension_Register_reg,(ByteRead(Extension_Register_reg) & 0b11101111));
}


//---------------------------
void RX8900RTC::SET_TSEL_1S(void) {      //SET TIMER INTERVAL 1sec
  ByteWrite(Extension_Register_reg,((ByteRead(Extension_Register_reg) & 0b11111100)|0b00000010));
}


void RX8900RTC::SET_TSEL(SOURCE_CLOCK_TYPES_t sourceClock) {         //SET TIMER INTERVAL
  ByteWrite(Extension_Register_reg,(ByteRead(Extension_Register_reg) & 0b11111100) | sourceClock); // set Count down cycle (source clock)
}


//---------------------------
void RX8900RTC::SET_UIE(void) {          //UPDATE INTERRUPT ENABLE
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) | 0b00100000));
}


//---------------------------
void RX8900RTC::RESET_UIE(void) {        //UPDATE INTERRUPT DISABLE
  ByteWrite(Control_Register_reg,(ByteRead(Control_Register_reg) & 0b11011111));
}


//---------------------------
void RX8900RTC::RESET_UF(void) {         //RESET UPDATE FLAG
  ByteWrite(Flag_Register_reg,(ByteRead(Flag_Register_reg) & 0b11011111));
}


//---------------------------
uint8_t RX8900RTC::IS_UF(void) {            //IS UPDATE INTERRUPT?
  return (ByteRead(Flag_Register_reg) & 0b00100000);
}


//---------------------------
void RX8900RTC::SET_USEL(USEL_t ust) {      //SET UPDATE INTERRUPT SELECT
  ByteWrite(Extension_Register_reg,((ByteRead(Extension_Register_reg) & 0b11011111)|ust));
}


// IS VLF (Voltage Low Flag) ON? TRUE: Supply voltage drop less than 1.6V or oscillation stopped.
// See Application Manual for details.
bool RX8900RTC::IS_VLF(void) {
  return (ByteRead(Flag_Register_reg) & 0b00000010);
}


// IS VDET (Voltage Detection Flag) FLAG ON? TRUE: Supply voltage drop less than 1.95V.
// See Application Manual for details.
bool RX8900RTC::IS_VDET(void) {
  return (ByteRead(Flag_Register_reg) & 0b00000001);
}

// Zeller’s Congruence; Find the Day for a Date
// 0: Sunday, 6:Saturday
// https://edu.clipper.co.jp/pg-2-47.html
uint8_t RX8900RTC::subZeller( uint16_t y, uint16_t m, uint16_t d ) {
  if( m < 3 ) {
    y--; m += 12;
  }
  return ( y + y/4 - y/100 + y/400 + ( 13*m + 8 )/5 + d )%7;
}


/*----------------------------------------------------------------------*
 * Decimal-to-BCD conversion                                            *
 *----------------------------------------------------------------------*/
uint8_t RX8900RTC::dec2bcd(uint8_t n) {
  return n + 6 * (n / 10);
//  return ((n / 10) & 0x0f) << 4 | ((n % 10) & 0x0f); // slower
}


/*----------------------------------------------------------------------*
 * BCD-to-Decimal conversion                                            *
 *----------------------------------------------------------------------*/
uint8_t __attribute__ ((noinline)) RX8900RTC::bcd2dec(uint8_t n) {
  return n - 6 * (n >> 4);
//  return ((n >> 4) & 0x0f) * 10 + (n & 0x0f); // slower
}

//RX8900RTC RTC = RX8900RTC();            //instantiate an RTC object
