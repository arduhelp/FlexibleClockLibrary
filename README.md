# FlexibleClockLibrary v1.1+
 [українською](README.md) / [english](README_EN.md)
---
![GitHub Release](https://img.shields.io/github/v/release/arduhelp/FlexibleClockLibrary?style=flat-square) 
![platform](https://img.shields.io/badge/platform-esp32,_esp8266-blue) [![YouTube](https://img.shields.io/badge/YouTube-Video-gold)](https://www.youtube.com/channel/ваш_канал)

> mini OS & toolkit for esp32/8266 based devices

![IMAGE.PNG](IMAGE.PNG)

# **Що це?**

Flexible Clock Library (FCL os) - це прошивка з набором інструментів для створення пристроїв та смарт годинників на базі esp32/8266. Вона підтримує графічний інтерфейс , wifi , Bluetooth , IR та ще багато іншого що можна додати завдяки модульній структурі.

---

# **Зміст**

- [Що це?](#що-це)
- [Початок](#get-started)
- [Додати плату](#1-додати-підтримку-плат-в-іде)
- [Встановити бібліотеку](#2-встановити-бібліотеку)
- [Завантажити прошивку](#3-завантажте-базову-прошивку)
- [Готово](#-готово)

---

# Get started

## 1. Додати підтримку плат в ide

> ESP32 --------------------

1. Відкрий Arduino IDE

2. Зайди в `File > Preferences`

3. В полі "Additional Board URLs" встав:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

4. Перейди до `Tools > Board > Board Manager`

5. Знайди ESP32 by Espressif та встанови

6. Оберіть цю плату

> ESP8266 --------------------

1. Зайди в `File > Preferences`

2. Додай ще одну URL:

```
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

3. Перейди в `Tools > Board > Board Manager`

4. Знайди ESP8266 by ESP8266 Community та встанови

5. Оберіть цю плату `LOLIN(WEMOS) D1 R2 & mini`

## 2. **Встановити бібліотеку**

1. Перейди на `GitHub репозиторій > релізи`:

https://github.com/arduhelp/FlexibleClockLibrary/releases

2. Натисни `Code > Download ZIP` або завантаж .bin файл `releases > assets > fcl.bin`

3. Підключи бібліотеку

1. В Arduino IDE відкрий `Sketch > Include Library > Add .ZIP Library...`

2. Вибери FlexibleClockLib.zip або вручну додану папку

## 3. **Завантажте базову прошивку**

1. В Arduino IDE зайди в `File > Examples > FlexibleClockLibrary`

2. Відкрий _fcl-sketch.ino_ або інший демо-проект

3. підлаштуйте власні піни
```cpp
FlexibleClockLibrary(U8G2& disp, OKpin, OKsig, analogButton_ pin, ssidConfig, passwordConfig, IR_tx_pin, IR_rx_pin, mHz_tx_pin, vibroPin);
```

4. Завантаж на свою плату

---

# **✅ Готово!**

Тепер FCL OS готовий до роботи на твоєму годиннику або проекті на ESP!

---