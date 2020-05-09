#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <TimeLib.h>
#include <Wire.h>
#include <RX8900RTC.h>

RX8900RTC RTC;

#define ALARM_INT_PIN 2

volatile boolean rtcint = false;   // Interrupt event flag

void setup() {
  Wire.begin();
  Serial.begin(9600);
  RTC.init();
  RTC.disableAlarm();
  RTC.disableFixedCycleTimer();
  RTC.timeUpdateTimerInterrupt(DISABLE); // Time Update Timer cannot be fully stopped. 
  
  pinMode(ALARM_INT_PIN, INPUT_PULLUP);  // interrupt input pin should be input mode to wake up from sleep mode

// set one of the alarm setting 
// alarm //
  setMinuteAlarmInt(0);              // set alarm to fire at 0 minute every hour.
//  setMinuteHourAlarmInt(0, 12);      // set alarm to fire at 12:00 every day.
//  setDayOfMonthAlarmInt(0, 12, 20);  // set alarm to fire at 12:00 on 20th every month. 
//  setWeekAlarmInt(0, 12, SUN | SAT); // set alarm to fire at 12:00 on every Sunday and Saturday.

// set one of the time update timer //
//  setUpdateSecondInt();              // set timer to fire every second update.
  setUpdateMinuteInt();              // set timer to fire every minute update.

// set fixed cycle timer
  setFixedCycleInt(7, SECOND_UPDATE);// set timer to fire every 7 seconds interval.

// You can use alarm ,time update timer, and fixed cycle timer togather.
// You can identify what event has occured by alarm() fixedCycleTimer, and timeUpdateTimer().
// See Application Manual for details.
//  https://www5.epsondevice.com/ja/products/rtc/rx8900sa.html (Japanese)
//  https://www5.epsondevice.com/en/products/rtc/rx8900ce.html (English)
}

void loop() {
  if (rtcint) { // interrupt call occured
    // do alarm job
    tmElements_t tm = RTC.read();
    Serial.println();
    Serial.println("Interrupt call");
    rtcint = false;
    alarmCheck(tm);
    fixedCycleTimerCheck(tm);
    timeUpdatetimerCheck(tm);
  }
//  enterSleep();
}



// After alarm interrupt, AF (Alarm Flag) become "1" and is retained until reset.
// /INT pin become LOW and retained until reset.
// You have to reset AF manually to detect next alarm event or set alarm again.
void alarmCheck(tmElements_t tm) {
  if (RTC.alarm()) { // Check AF (Alarm Flag) and resets AF for next alarm event detection.
    Serial.print("Alarm fired on ");
    serialTime(tm);
  }
}


// When fixed cycle Timer interrupt occured, TF (Timer Flag) become "1" and is retained until reset,
// but /INT pin status is automatically cleared earliest 7.813 ms after the interrupt occurs.
// See Application Manual for details.
void fixedCycleTimerCheck(tmElements_t tm) {
  if (RTC.fixedCycleTimer()) { // Reset TF (Timer Flag) for next timer event detection.
    Serial.print("Fixed Cycle Timer fired on ");
    serialTime(tm);
  }
}


// When time update interrupt occured, UF (Update Flag) become "1" and is retained until reset,
// but /INT pin status is automatically cleared earliest 7.813 ms after the interrupt occurs.
// See Application Manual for details.
void timeUpdatetimerCheck(tmElements_t tm) {
  if (RTC.timeUpdateTimer()) { // Reset UF (Update Flag) for next timer event detection.
    Serial.print("Time Update Timer fired on ");
    serialTime(tm);
  }
}

void alcall() {
  rtcint = true;
}

/////// set alarm and interrupt call /////////////////

void setMinuteAlarmInt(byte minute) {
  RTC.setAlarm(minute);
  Serial.print("Alarm is set to ");
  Serial.print(minute);
  Serial.println(" minute every  hour");
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.alarmInterrupt(ENABLE);
}


void setMinuteHourAlarmInt(byte minute, byte hour) {
  RTC.setAlarm(minute, hour);
  Serial.print("Alarm is set to ");
  Serial.print(hour);
  Serial.print(":");
  Serial.println(minute);
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.alarmInterrupt(ENABLE);
}


void setDayOfMonthAlarmInt(byte minute, byte hour, byte dayOfMonth) {
  RTC.setDayAlarm(minute, hour, dayOfMonth);
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.alarmInterrupt(ENABLE);
}


void setWeekAlarmInt(byte minute, byte hour, byte weekMask) {
  RTC.setWeekAlarm(minute, hour, weekMask);
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.alarmInterrupt(ENABLE);
}


//set timer to fire every second update
void setUpdateSecondInt() {
  RTC.setTimeUpdateTimer(UPDATE_SECOND_INT); // set update interrupt timing to every second
  Serial.println("Time update interrupt is set to SECOND UPDATE");
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.timeUpdateTimerInterrupt(ENABLE);
}

//set timer to fire every minute update
void setUpdateMinuteInt() {
  RTC.setTimeUpdateTimer(UPDATE_MINUTE_INT); // set update interrupt timing to every second
  Serial.println("Time update interrupt is set to MINUTE UPDATE");
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.timeUpdateTimerInterrupt(ENABLE);
}

//set timer to fire every interval time
void setFixedCycleInt(int timerCounter, SOURCE_CLOCK_TYPES_t sourceClock) {
  RTC.setFixedCycleTimer(timerCounter, sourceClock); //
  Serial.println("Fixed cycle Timer is set.");
  attachInterrupt(digitalPinToInterrupt(ALARM_INT_PIN), alcall, FALLING);
  RTC.fixedCycleTimerInterrupt(ENABLE);
}


void serialTime(tmElements_t tm){
  Serial.print(tmYearToCalendar(tm.Year), DEC);
  Serial.print(F("/"));
  Serial.print(tm.Month, DEC);
  Serial.print(F("/"));
  Serial.print(tm.Day, DEC);
  Serial.print(F(","));
  Serial.print(tm.Hour, DEC);
  Serial.print(F(":"));
  Serial.print(tm.Minute, DEC);
  Serial.print(F(":"));
  Serial.println(tm.Second, DEC);
}

void enterSleep(void) {
//  set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the alarm or timer event occured*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}
