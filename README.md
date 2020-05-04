# RX8900RTC
 Arduino library that supports the SEIKO EPSON RX8900 Real-Time Clocks
 v1.0  Citriena Mar 2019

このArduino用ライブラリはセイコーエプソンのSEIKO EPSON RX8900 Real Time Clock Module用に作成したものです。
実際に使用しているのは、秋月電子通商の「高精度ＲＴＣ（リアルタイムクロック）　ＲＸ８９００　ＤＩＰ化モジュール」
http://akizukidenshi.com/catalog/g/gK-13009/　です。

Arduino用のRTCモジュール、およびそのライブラリは他にもいろいろありますが、私が使って見た限りではLEDを外してもなぜか数百uAと
電池駆動で数カ月～年単位で使うには消費電流が無視できない大きさのものばかりでした。SEIKO EPSON RX8900 Real Time Clock Moduleは
数uA以下と消費電流が非常に小さいのは確認できましたが、目的にあうArduinoライブラリが見つかりませんでしたので、自作しました。

ベースは秋月電子通商で提供されているサンプルスケッチで、それをもとにライブラリ化しました。
アプリケーションマニュアルにある一通りの機能は一応実装したつもりです。

タイマーに関する個々の機能についてはRX8900RTCのアプリケーションマニュアルを見てもらう方が確実ですので、そちらを参照してください。
https://www5.epsondevice.com/ja/products/rtc/rx8900sa.html
