# RX8900RTC
 Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks

citriena

I'm not good at English so correcting my English is welcome when it is wrong.

## Introduction
**RX8900RTC** is an Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks.

RX8900 has features of high stability (±5 seconds of month deviation with UA class) and low current consumption (typically 0.7uA when Vdd=3V, Fout=OFF).

This library is baed on the [sample sketch by AKIZUKI DENSHI TSUSHO CO.,LTD.]( http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip) for [AE-RX8900 module](http://akizukidenshi.com/catalog/g/gK-13009/).

## Features
### Alarm
The alarm interrupt generation function generates interrupt events for alarm settings such as date, day, hour, and
minute settings.
### Fixed Cycle Timer
The fixed-cycle timer interrupt function generates interrupt events periodically at any fixed cycle set
between 244.14us and 4095 minutes.
### Time Update Interrupt
The time update interrupt function generates interrupt events at one-second or one-minute intervals.

## Constructor

```
RX8900RTC myRTC;
```

## Functions
### Initialize
```
void init(void);
void begin(void);
```
Initialize RX8900. Eatehr of the functions above is available.

### Setting the RTC time

```
uint8_t set(time_t t);
uint8_t write(tmElements_t tm);
```

### Reading the RTC time
```
static time_t get(void);
static tmElements_t read(void);
```

### Alarm

#### Setting alarm
***
```
void setAlarm(uint8_t minute);
void setAlarm(uint8_t minute, uint8_t hour);
```
* minute: The minute value to set the alarm to.
* hour: The hour value to set the alarm to.
***
```
void setDayAlarm(uint8_t minute, uint8_t hour, uint8_t daydate);
```
* daydate: The date of the month to set the alarm to.
***
```
void setWeekAlarm(uint8_t minute, uint8_t hour, uint8_t daydate);
```
* daydate: The day(s) of the week to set the alarm to. The days are SUN, MON, TUE, WED, THU, FRI,and SAT.

 Multiple days of the week can be set as SUN | SAT
***
#### Other functions
***
```
void alarmInterrupt(INTERRUPT_CONTROL_t interrupt);
```
Specifies /INT status of RX8900 when an alarm interrupt event occurs.

interrupt:
 * ENABLE：/INT status changes from HIGH to LOW
 * DISABLE：/INT status do not change (remains HIGH).
***
```
bool alarm(void)
```
* Returns true when AF (Alarm Flag) is "1". Otherwise (AF is "0") returns false.
* When AF is "1", writes "0" to clear AF. /INT will changes to HIGH if LOW.
***
```
void disableAlarm(void);
```
Stops alarm function.
***
#### When an alarm event occurs
* AF (Alarm Flag) value changes from "0" to 1". AF value is retaind until cleared manually.

 You can check the AF and clear it with "alarm(void)".
* /INT status changes to LOW if "alarmInterrupt(ENABLE)" is used. /INT status is retained until AF is cleared manually.

### Fixed Cycle Timer
The fixed-cycle timer generates interrupt events periodically at a fixed cycle between 244.14us and 4095 minutes.

#### Functions
***
```
void setFixedCycleTimer(uint16_t timerCounter, SOURCE_CLOCK_TYPES_t sourceClock);
```
Set the fixed-cycle timer interval.
* timerCounter：0~4095
* sourceClock:
  * CLOCK_4096HZ：4096Hz
  * CLOCK_64HZ：64Hz
  * SECOND_UPDATE：1Hz (Once per second)
  * MINUTE_UPDATE：1/60Hz (Once per minute)

The fixed-cycle timer interval is [sourceClock] x [timerCounter].
***
```
void fixedCycleTimerInterrupt(INTERRUPT_CONTROL_t interrupt);
```
Specifies /INT status of RX8900 when a fixed-cycle timer interrupt event occurs.

interrupt:
 * ENABLE：/INT status changes from HIGH to LOW
 * DISABLE：/INT status do not change (remains HIGH).
***
```
bool fixedCycleTimer(void);
```
* Returns true when TF(Timer Flag) is "1". Otherwise (TF is "0") returns false.
* When TF is "1" write "0" to clear TF.
***
```
void disableFixedCycleTimer(void);
```
Stops fixed-cycle timer function.
***
#### When a fixed-cycle timer interrupt event occurs
* TF(Timer Flag) value changes from "0" to 1". TF value is retaind until cleared manually.

 You can check the TF and clear it with "fixedCycleTimer(void)".
* /INT status changes LOW if "fixedCycleTimerInterrupt(ENABLE)" is used. /INT status is automatically cleared 7.813ms or 722us (sourceClock is CLOCK_4096HZ) after the event occured.

#### Timing of counting down and interrupts of the fixed-cycle timer
* When the source clock has been set to "SECOND_UPDATE" or "MINUTE_UPDATE", the timing of both countdown and interrupts is coordinated with the internal clock update timing.
* The first countdown timing is therefore not exact one sourceClock cycle from the start. The first fixedCycleTimer interrupt event will occure ealier than fixed-cycle.
### Time Update Interrupt
The time update interrupt function generates interrupt events at one-second or one-minute intervals.

The events will occure according to the timing of the internal clock update.

The time update interrupt function can not be fully stopped.

#### Functions
***
```
void setTimeUpdateTimer(USEL_t usel);
```
Set time update timer interval.

usel:
 * UPDATE_SECOND_INT: interrupt event will occure once per second.
 * UPDATE_MINUTE_INT: interrupt event will occure once per minute.
***
```
void timeUpdateTimerInterrupt(INTERRUPT_CONTROL_t interrupt);
```
Specifies /INT status of RX8900 when a time update interrupt event occurs.

interrupt:
 * ENABLE：/INT status changes from HIGH to LOW
 * DISABLE：/INT status do not change (remains HIGH).
***
```
bool timeUpdateTimer(void);
```
* Returns true when UF (Update Flag) is "1". Otherwise (UF is "0") returns false.
* When UF is "1" write "0" to cleart UF.
***
#### When a time update timer interrupt event occurs

* UF(Updater Flag) value changes from "0" to 1". UF value is retaind until cleared manually.

 You can check the UF and clear it with "timeUpdateTimer(void)".
* /INT status changes LOW if "timeUpdateTimerInterrupt(ENABLE)" is used. /INT status is automatically cleared 15.63ms (usel is UPDATE_MINUTE_INT) or 500ms (usel is UPDATE_SECOND_INT) after the event occured.

### Other functions
```
float temperature(void);
```
Returns internal temperature sensor value in Celcius.

## Usage
See sample skeches for basic usage.

See Application Manual for details of each function.
https://www5.epsondevice.com/en/products/rtc/rx8900ce.html

## Releases

### 1.0.0 - Mar  4, 2019

### 1.0.1 - May 23, 2020
* rename some functions constants and variables

### 1.0.2 - Mov 05, 2023
* bug fix: modify WEEK_TYPES_t definition
