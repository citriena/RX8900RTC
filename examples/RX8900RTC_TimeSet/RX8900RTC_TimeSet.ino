#include <Arduino.h>
#include <Wire.h>
#include <TimeLib.h>    // https://github.com/PaulStoffregen/Time
#include <RX8900RTC.h>  // https://github.com/citriena/RX8900RTC

RX8900RTC RTC;


void setup() {
  Wire.begin();
  Serial.begin(9600);
  RTC.init();
  RTC.set(compileTime());  // set compiled time to RTC
//  tmElements_t tm = {0, 0, 12, 0, 5, 5, CalendarYrToTm(2020)}; // second, minute, hour, week, day, month, year
//  RTC.write(tm);
}

void loop() {
  serialTime(RTC.read());
  delay(1000);
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

// function to return the compile date and time as a time_t value
// from alarm_ex1.ino in Arduino DS3232RTC Library sample sketch by Jack Christensen.
time_t compileTime()
{
    const time_t FUDGE(10);    //fudge factor to allow for upload time, etc. (seconds, YMMV)
    const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char compMon[4], *m;

    strncpy(compMon, compDate, 3);
    compMon[3] = '\0';
    m = strstr(months, compMon);

    tmElements_t tm;
    tm.Month = ((m - months) / 3 + 1);
    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);

    time_t t = makeTime(tm);
    return t + FUDGE;        //add fudge factor to allow for compile time
}
