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
RX8900RTC RTC;
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
```
void setAlarm(uint8_t minute);
void setAlarm(uint8_t minute, uint8_t hour);
```
* minute: The second value to set the alarm to.
* hour: The hour value to set the alarm to.

```
void setDayAlarm(uint8_t minute, uint8_t hour, uint8_t daydate);
```
* daydate: The date of the month to set the alarm to.

```
void setWeekAlarm(uint8_t minute, uint8_t hour, uint8_t daydate);
```
* daydate: The day(s) of the week to set the alarm to. The days are SUN, MON, TUE, WED, THU, FRI,and SAT.

 Multiple days of the week can be set as SUN | SAT

#### Other functions
```
void alarmInterrupt(INTERRUPT_CONTROL_t interrupt);
```
Specifies /INT status of RX8900 when an alarm interrupt event occurs.

interrupt:
 * ENABLE：/INT status changes from HIGH to LOW
 * DISABLE：/INT status do not change (remains HIGH).

```
bool alarm(void)
```
* Returns true when AF (Alarm Flag) is "1". Otherwise (AF is "0") returns false.
* When AF is "1", writes "0" to clear AF. /INT will changes to HIGH if LOW.

```
void disableAlarm(void);
```
Stops alarm function.

#### When an alarm event occurs
* AF (Alarm Flag) value changes from "0" to 1". AF value is retaind until cleared manually.

 You can check the AF and clear it with "alarm(void)".
* /INT status changes to LOW if "alarmInterrupt(ENABLE)" is used. /INT status is retained until AF is cleared manually.

### Fixed Cycle Timer
The fixed-cycle timer generates interrupt events periodically at a fixed cycle between 244.14us and 4095 minutes.

#### Functions
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

```
void fixedCycleTimerInterrupt(INTERRUPT_CONTROL_t interrupt);
```
Specifies /INT status of RX8900 when a fixed-cycle timer interrupt event occurs.

interrupt:
 * ENABLE：/INT status changes from HIGH to LOW
 * DISABLE：/INT status do not change (remains HIGH).

```
bool fixedCycleTimer(void);
```
* Returns true when TF(Timer Flag) is "1". Otherwise (TF is "0") returns false.
* When TF is "1" write "0" to clear TF.

```
void disableFixedCycleTimer(void);
```
Stops fixed-cycle timer function.

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
```
void setTimeUpdateTimer(USEL_t usel);
```
Set time update timer interval.

usel:
 * UPDATE_SECOND_INT: interrupt event will occure once per second.
 * UPDATE_MINUTE_INT: interrupt event will occure once per minute.

```
void timeUpdateTimerInterrupt(INTERRUPT_CONTROL_t interrupt);
```
Specifies /INT status of RX8900 when a time update interrupt event occurs.

interrupt:
 * ENABLE：/INT status changes from HIGH to LOW
 * DISABLE：/INT status do not change (remains HIGH).

```
bool timeUpdateTimer(void);
```
* Returns true when UF (Update Flag) is "1". Otherwise (UF is "0") returns false.
* When UF is "1" write "0" to cleart UF.

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
* renamed some functions constants and variables


## 概要
このArduino用ライブラリはセイコーエプソンのSEIKO EPSON RX8900 Real Time Clock Module用に作成したものです。実際に使用しているのは、秋月電子通商の[高精度ＲＴＣ（リアルタイムクロック）　ＲＸ８９００　ＤＩＰ化モジュール](http://akizukidenshi.com/catalog/g/gK-13009/) です。RX8900は高精度（UAタイプで月差±5秒以内）、低消費電流（Typ. 0.7uA @3V)と高性能のRTCです。

Arduino用のRTCモジュール、およびそのライブラリは他にもいろいろありますが、私が使って見た限りではLEDを外しても数100uAと電池駆動で数カ月～年単位で使うには消費電流が無視できない大きさのものばかりでした。SEIKO EPSON RX8900 Real Time Clock Moduleはデータシート通り数uA以下と消費電流が非常に小さいのは確認できましたが、目的にあうArduinoライブラリが見つかりませんでしたので自作しました。

ベースは秋月電子通商で提供されている[サンプルスケッチ](http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip)で、それをもとにライブラリ化しました。

RX8900のアプリケーションマニュアルにある一通りの機能は一応実装したつもりです。機能は大きく分けて以下の３つです。
* アラーム
* 定周期タイマー
* 時刻更新割り込み

## コンストラクタ
```
RX8900RTC RTC;
```
設定はありません。

## 機能
個々の機能について簡単に説明します。詳細についてはRX8900のアプリケーションマニュアルを参照してください。
https://www5.epsondevice.com/ja/products/rtc/rx8900sa.html

具体的な使い方はサンプルスケッチを参照してください。

### 初期化
次のどちらかを実行します。
```
void init(void);
void begin(void);
```

### 時刻

#### 取得
日時を格納する変数として time_t型を使う方法と tmElements_t型を使う方法があります。

```
static time_t get(void);
static tmElements_t read(void);
```
static にしているのはtime.libの setSyncProvider() 等に対応するためです。

#### 設定
こちらも日時を格納する変数としてtime_t型 を使う方法と tmElements_t型を使う方法があります。
```
uint8_t set(time_t t);
uint8_t write(tmElements_t tm);
```
返り値は将来の拡張に備えたもので、現在は常に0を返します。

### アラーム
[分]、[時]、[曜]、[日]などに対する割り込みイベントを発生させる機能です。

#### 設定
```
void setAlarm(uint8_t minute);
```
minuteで[分]を設定します。毎時、設定した[分]になると割り込みイベントが発生します。
```
void setAlarm(uint8_t minute, uint8_t hour);
```
minuteで[分]、hourで[時]を設定します。毎日、設定した[時][分]になると割り込みイベントが発生します。
```
void setDayAlarm(uint8_t minute, uint8_t hour, uint8_t daydate);
```
minuteで[分]、hourで[時]、daydateで[日]を設定します。毎月、設定した[日][時][分]になると割り込みイベントが発生します。
```
void setWeekAlarm(uint8_t minute, uint8_t hour, uint8_t daydate);
```
minuteで[分]、hourで[時]、daydateで[曜]を設定します。毎週、設定した[曜][時][分]になると割り込みイベントが発生します。
[曜]は以下で設定します。
 * 日: SUN, 月: MON, 火: TUE, 水: WED, 木: THU, 金: FRI, 土: SAT

複数の[曜]が設定可能です。たとえば SUN | SAT とすれば日曜と土曜日に割り込みイベントが発生します。
```
void alarmInterrupt(INTERRUPT_CONTROL_t interrupt);
```
割り込みイベント発生時における/INT の動作を指定します。

interrupt には以下が設定できます。
 * ENABLE：割り込みイベント発生時に/INT を"LOW"にします。
 * DISABLE：割り込みイベント発生時に/INT は変化せず、"HIGH"のままです。

```
bool alarm(void);
```
* AF(Alarm Flag)を確認し、"1"だったらture , "0" だったらfalseを返します。
* AFが"1"だったら"0"を書き込み、リセットします。また、/INT がLOWだったらHIGHにリセットされます。

```
void disableAlarm(void);
```
アラームを無効にします。

#### 割り込みイベント発生時
 * AF(Alarm Flag)が"1"になります。AFは手動でリセットしない限り保持されます。alarm(void) でAFの確認、および"1"だったらリセットが行えます。
 * alarmInterrupt(ENABLE) を実行していれば /INT は"LOW" になります。 /INT出力は手動解除しない限り保持されます。alarm(void)でAFをリセットすると /INTもリセット（"HIGH"）されます。

### 定周期タイマー
定期的な割り込みイベントを発生させる機能です。設定できる定周期は244.14us～4095minです。

#### 設定
定周期は、ソースクロックとタイマーカウンターで設定します。
```
void setFixedCycleTimer(uint16_t timerCounter, SOURCE_CLOCK_TYPES_t sourceClock);
```
* timerCounter(タイマーカウンター)：0～4095
* sourceClock（ソースクロック）:
  * CLOCK_4096HZ：4096Hz
  * CLOCK_64HZ：64Hz
  * SECOND_UPDATE：1Hz（1秒周期）
  * MINUTE_UPDATE：1/60Hz（1分周期）

が設定できます。ソースクロック毎にタイマーカウンターがカウントダウンしていき、0になったら割り込みイベントが発生します。このため定周期は、 ソースクロック × タイマーカウンター となります。たとえばソースクロックを1秒周期、タイマーカウンターを13とすれば、13秒ごとに割り込みイベントが発生します。割り込みイベントが発生するとタイマーカウンターは自動的に設定値に戻り、繰り返し動作します。
```
void fixedCycleTimerInterrupt(INTERRUPT_CONTROL_t interrupt);
```
割り込みイベント発生時における/INT の動作を指定します。

interrupt には以下が設定できます。
 * ENABLE：割り込みイベント発生時に/INT を"LOW"にします。
 * DISABLE：割り込みイベント発生時に/INT は変化せず、"HIGH"のままです。

```
bool fixedCycleTimer(void);
```
TF(Timer Flag)を確認し、"1"だったらture , "0" だったらfalseを返します。また、"1"だったら"0"を書き込み、リセットします。

```
void disableFixedCycleTimer(void);
```
定周期タイマーを無効にします。

#### 割り込みイベント発生時
 * TF(Timer Flag)が"1"になります。TFは手動でリセットしない限り保持されます。fixedCycleTimer(void) でTFの確認、および"1"だったらリセットが行えます。
 * fixedCycleTimerInterrupt(ENABLE) を実行していれば /INT は"LOW" になります。 /INT出力は ソースクロックがCLOCK_4096HZの場合は122us、それ以外は7.813msで自動解除されます。

#### カウントダウンのタイミング
 * ソースクロック1Hz（1秒周期）時のカウントダウンは､内部計時の[秒]更新に連動します。このため、最初の割り込みイベント発生は定周期から最大1秒程度短くなります。
 * ソースクロック 1/60Hz（1分周期）時のカウントダウンは内部計時の[分]更新に連動します。このため、最初の割り込みイベント発生は定周期から最大1分程度短くなります。

 いずれも、2回目の割り込みイベント以降は設定した定周期となります。

### 時刻更新割り込み
1秒更新、または1分更新にて内部計時に連動したタイミングで割り込みイベントを発生させる機能です。この時刻更新割り込み機能は停止できません。

#### 設定
```
void setTimeUpdateTimer(USEL_t usel);
```
usel:
 * UPDATE_SECOND_INT = 1秒更新
 * UPDATE_MINUTE_INT = 1分更新

```
void timeUpdateTimerInterrupt(INTERRUPT_CONTROL_t interrupt);
```
割り込みイベント発生時における/INT の動作を指定します。

interrupt には以下の設定が行えます。
 * ENABLE：割り込みイベント発生時に/INT を"LOW"にします。
 * DISABLE：割り込みイベント発生時に/INT は変化せず、"HIGH"のままです。

```
bool timeUpdateTimer(void);
```
UF(Update flag)を確認し、"1"だったらture , "0" だったらfalseを返します。また、"1"だったら"0"を書き込み、リセットします。

#### 割り込みイベント発生時
 * UF(Update flag)が"1"になります。UFは手動でリセットしない限り保持されます。timeUpdateTimer(void) でUFの確認、および"1"だったらリセットが行えます。
 * timeUpdateTimerInterrupt(ENABLE) を実行していれば /INT は"LOW" になります。 /INT出力は 15.63ms (1分更新時)または500ms（1秒更新時）で自動解除されます。

### その他の機能
```
float temperature(void);
```
内部の温度センサの値(℃)を返します。
