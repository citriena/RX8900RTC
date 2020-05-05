# RX8900RTC
 Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks
 v1.0  Citriena Mar 2019

## Introduction
**RX8900RTC** is an Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks.
This library is baed on the sample sketch by AKIZUKI DENSHI TSUSHO CO.,LTD.
 http://akizukidenshi.com/catalog/g/gK-13009/
 http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip
 Some codes are based on DS3232RTC.CPP by Jack Christensen 06Mar2013

## Usage
See sample skech for basic usage of alarm and timer.

See Application Manual for details of each function.
https://www5.epsondevice.com/en/products/rtc/rx8900ce.html


このArduino用ライブラリはセイコーエプソンのSEIKO EPSON RX8900 Real Time Clock Module用に作成したものです。
実際に使用しているのは、秋月電子通商の「高精度ＲＴＣ（リアルタイムクロック）　ＲＸ８９００　ＤＩＰ化モジュール」
http://akizukidenshi.com/catalog/g/gK-13009/　です。

Arduino用のRTCモジュール、およびそのライブラリは他にもいろいろありますが、私が使って見た限りではLEDを外してもなぜか数百uAと
電池駆動で数カ月～年単位で使うには消費電流が無視できない大きさのものばかりでした。SEIKO EPSON RX8900 Real Time Clock Moduleは
数uA以下と消費電流が非常に小さいのは確認できましたが、目的にあうArduinoライブラリが見つかりませんでしたので、自作しました。

ベースは秋月電子通商で提供されているサンプルスケッチで、それをもとにライブラリ化しました。
  http://akizukidenshi.com/download/ds/akizuki/RX8900_SAMPLE.zip
RX8900のアプリケーションマニュアルにある一通りの機能は一応実装したつもりです。

基本的な使い方はサンプルスケッチを参照してください。

タイマーに関する個々の機能の詳細についてはRX8900のアプリケーションマニュアルを見てもらう方が確実ですので、そちらを参照してください。
https://www5.epsondevice.com/ja/products/rtc/rx8900sa.html
