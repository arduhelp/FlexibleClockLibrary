# FlexibleClockLibrary v1.1+
[українською](README.md) / [english](README_EN.md)

---
![GitHub Release](https://img.shields.io/github/v/release/arduhelp/FlexibleClockLibrary?style=flat-square) 
![platform](https://img.shields.io/badge/platform-esp32,_esp8266-blue) [![YouTube](https://img.shields.io/badge/YouTube-Video-gold)](https://www.youtube.com/@etar)

> Mini OS & toolkit for esp32/8266 based devices

![IMAGE.PNG](https://github.com/arduhelp/FlexibleClockLibrary/blob/main/Design/imageWatch_en.png)

# **What is it?**

Flexible Clock Library (FCL OS) is a firmware and toolkit for building smart clocks and devices based on ESP32/8266.  
It supports a graphical user interface, WiFi, Bluetooth, IR, and many other features thanks to its modular design.

---

# **Contents**

- [What is it?](#what-is-it)
- [Get Started](#get-started)
- [Add Board Support](#1-add-board-support-in-arduino-ide)
- [Install the Library](#2-install-the-library)
- [Upload the Firmware](#3-upload-the-base-firmware)
- [Done](#️-done)

---

# Get Started

## 1. Add board support in Arduino IDE

> For ESP32

1. Open Arduino IDE  
2. Go to `File > Preferences`  
3. In the "Additional Board URLs" field, paste:

https://espressif.github.io/arduino-esp32/package_esp32_index.json

4. Go to `Tools > Board > Board Manager`  
5. Search for **ESP32 by Espressif** and install it  
6. Оберіть цю плату `ESP32C3 Dev Module`
7. Перейди та Обери `Partition Scheme > "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"`

> For ESP8266
❗ Will NOT be supported above v1.1s2 ❗
1. Go to `File > Preferences`  
2. Add another URL:

http://arduino.esp8266.com/stable/package_esp8266com_index.json

3. Go to `Tools > Board > Board Manager`  
4. Search for **ESP8266 by ESP8266 Community** and install it  
5. Select the board `Wemos D1 mini`

## 2. **Install the Library**

1. Go to the `GitHub repository > Releases`:  
https://github.com/arduhelp/FlexibleClockLibrary/releases

2. Click `Code > Download ZIP` or download the `.bin` file from `releases > assets > fcl.bin`

3. Add the library:

   1. In Arduino IDE, go to `Sketch > Include Library > Add .ZIP Library...`  
   2. Select `FlexibleClockLib.zip` or the manually added folder

## 3. **Upload the Base Firmware**

1. In Arduino IDE go to `File > Examples > FlexibleClockLibrary`  
2. Open _fcl-sketch.ino_ or another demo project  
3. Configure your custom pins:

```cpp
FlexibleClockLibrary(U8G2& disp, OKpin, OKsig, analogButton_pin, ssidConfig, passwordConfig, IR_tx_pin, IR_rx_pin, mHz_tx_pin, vibroPin);
```

4. Upload to your board




---

✅ Done!

Now FCL OS is ready to run on your ESP-powered watch or project!


---
