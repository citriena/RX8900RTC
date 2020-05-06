# RX8900RTC
 Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks
 v1.0  Citriena Mar 2019

## Introduction
**RX8900RTC** is an Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks.
This library is baed on the sample sketch by AKIZUKI DENSHI TSUSHO CO.,LTD.
 http://akizukidenshi.com/catalog/g/gK-13009/
 http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip
 Some codes are based on DS3232RTC.CPP by Jack Christensen 06Mar2013

## Function
### Alarm
The alarm interrupt generation function generates interrupt events for alarm settings such as date, day, hour, and
minute settings.
### Fixed Cycle Timer
The fixed-cycle timer interrupt generation function generates an interrupt event periodically at any fixed cycle set
between 244.14us and 4095 minutes.
### Time Update Interrupt
The time update interrupt function generates interrupt events at one-second or one-minute intervals, according to
the timing of the internal clock.
## Usage
See sample skech for basic usage.

See Application Manual for details of each function.
https://www5.epsondevice.com/en/products/rtc/rx8900ce.html

## 概要
このArduino用ライブラリはセイコーエプソンのSEIKO EPSON RX8900 Real Time Clock Module用に作成したものです。実際に使用しているのは、秋月電子通商の「高精度ＲＴＣ（リアルタイムクロック）　ＲＸ８９００　ＤＩＰ化モジュール」  http://akizukidenshi.com/catalog/g/gK-13009/ です。

Arduino用のRTCモジュール、およびそのライブラリは他にもいろいろありますが、私が使って見た限りではLEDを外してもなぜか数百uAと電池駆動で数カ月～年単位で使うには消費電流が無視できない大きさのものばかりでした。SEIKO EPSON RX8900 Real Time Clock Moduleは数uA以下と消費電流が非常に小さいのは確認できましたが、目的にあうArduinoライブラリが見つかりませんでしたので、自作しました。

ベースは秋月電子通商で提供されているサンプルスケッチで、それをもとにライブラリ化しました。
 http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip

RX8900のアプリケーションマニュアルにある一通りの機能は一応実装したつもりです。基本的な使い方はサンプルスケッチを参照してください。アラーム、タイマー等個々の機能の詳細についてはRX8900のアプリケーションマニュアルを参照してください。
https://www5.epsondevice.com/ja/products/rtc/rx8900sa.html

### アラーム
[分]、[時]、[曜]、[日]などに対する割り込みイベントを発生させる機能です。

#### 設定

##### void setAlarm(byte minutes);
minuteで[分]を設定します。毎時、設定した[分]になると割り込みイベントが発生します。

##### void setAlarm(byte minute, byte hour);
minuteで[分]、hourで[時]を設定します。毎日、設定した[時][分]になると割り込みイベントが発生します。

##### void setDayAlarm(byte minutes, byte hours, byte daydate);
minuteで[分]、hourで[時]、daydateで[日]を設定します。毎月、設定した[日][時][分]になると割り込みイベントが発生します。

##### void setWeekAlarm(byte minutes, byte hours, byte daydate);
minuteで[分]、hourで[時]、daydateで[曜]を設定します。毎週、設定した[曜][時][分]になると割り込みイベントが発生します。
[曜]は以下で設定します。
 * 日: SUN, 月: MON, 火: TUE, 水: WED, 木: THU, 金: FRI, 土: SAT

複数の[曜]が設定可能です。たとえば SUN | SAT とすれば日曜と土曜日に割り込みイベントが発生します。

#### 割り込みイベント発生時
 * AF(Alarm Flag)が"1"になります。AFは手動でリセットしない限り保持されます。bool alarmUp(void) でAFの確認、および"1"だったらリセットが行えます。
 * alarmInterrupt(INTERRUPT_ENABLE) を実行していれば /INT は"L" になります。 /INT出力は手動解除しない限り保持されます。bool alarmUp(void)でAFをリセットすると /INTもリセット（"H"）されます。

### 定周期タイマー
定期的な割り込みイベントを発生させる機能です。設定できる定周期は244.14us～4095minです。

#### 設定
定周期は、ソースクロックとタイマーカウンターで設定します。

##### void FixedCycleTimer(timerCounter, sourceClock)
 * timerCounter(タイマーカウンター)：0～4095
 * sourceClock（ソースクロック）：4096Hz、64Hz、1Hz（1秒周期）、1/60Hz（1分周期）の4種類

が設定できます。ソースクロック毎にタイマーカウンターがカウントダウンしていき、0になったら割り込みイベントが発生します。このため、定周期は、[ソースクロック]×[タイマーカウンター]となります。たとえば[ソースクロック]を1秒周期、タイマーカウンターを13とすれば、13秒ごとに割り込みイベントが発生します。割り込みイベントが発生するとタイマーカウンターは自動的に設定値に戻り、繰り返し動作します。


#### 割り込みイベント発生時
 * TF(Timer Flag)が"1"になります。TFは手動でリセットしない限り保持されます。bool fixedCycleTimerUp(void) でTFの確認、および"1"だったらリセットが行えます。
 * fixedCycleTimerInterrupt(INTERRUPT_ENABLE) を実行していれば /INT は"L" になります。 /INT出力は 7.813msで自動解除されます。

#### カウントダウンのタイミング
 * ソースクロック1Hz（1秒周期）時のカウントダウンは､内部計時の[秒]更新に連動しません。
 * ソースクロック 1/60Hz（1分周期）時のカウントダウンは内部計時の[分]更新に連動します。このため、最初の割り込みイベント発生は定周期から最大1分程度短くなります。2回目の割り込みイベント以降は設定した定周期となります。

### 時刻更新割り込み
1秒更新、または1分更新にて内部計時に連動したタイミングで割り込みイベントを発生させる機能です。この時刻更新割り込み機能は停止できません。なので、設定できるのは[1秒更新]、[1分更新]のどちらで割り込みを発生させるかです。

#### 設定

##### void setTimeUpdateTimer(USEL_t uTiming)

uTimingは
 * UPDATE_SECOND_INT = 1秒更新
 * UPDATE_MINUTE_INT = 1分更新

が設定できます。

#### 割り込みイベント発生時
 * UF(Update flag)が"1"になります。UFは手動でリセットしない限り保持されます。bool timeUpdateTimerUp(void) でUFの確認、および"1"だったらリセットが行えます。
 * timeUpdateTimerInterrupt(INTERRUPT_ENABLE); を実行していれば /INT は"L" になります。 /INT出力は 15.63ms (1分更新時)または500ms（1秒更新時）で自動解除されます。
