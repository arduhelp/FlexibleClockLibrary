#ifndef FLEXIBLECLOCKLIBRARY_h
#define FLEXIBLECLOCKLIBRARY_h

#include <avr/pgmspace.h>
#include <Arduino.h>
#include <U8g2lib.h>



#ifdef ESP32
    #include <WiFi.h>
    #include <WiFiClient.h>
    #include <WiFiAP.h>
  //  #include <WebServer.h>
  //  WebServer server(80);
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <WiFiClient.h>
   // #include <ESP8266WebServer.h>
  //  ESP8266WebServer server(80);
#else
    #error "not designed for this platform! esp32 or esp8266 is recommended!"
#endif

class FlexibleClockLibrary{
public:
    // Constructor
    FlexibleClockLibrary(U8G2& disp, uint8_t OKpin, uint8_t OKsig, uint8_t analogButton, const char* ssidConfig, const char* passwordConfig, uint8_t IR_tx, uint8_t IR_rx, uint8_t mHz_tx, uint8_t vibroPin);
    


    // Public Methods
    void begin();
    void clearDisp();
    void drawLines();
    //taskbar
    void taskbar_begin();
    void taskbar_draw(int taskbar_y = 8);
    void drawBitmape(const unsigned char* bitmape);
    
    static const unsigned char _err_bitmap_err[];
    static const unsigned char _err_bitmap_noerr[];
    static const unsigned char _err_bitmap_qestoin[];
    static const unsigned char _err_bitmap_loading [];
    // 'def_unknown', 8x8px
    static const unsigned char def_bitmap_def_unknown [];
    // 'wifi', 8x8px
    static const unsigned char wifi_bitmap_ap [];
    static const unsigned char wifi_bitmap_nothing [];
    static const unsigned char wifi_bitmap_wifi [];
    static const unsigned char wifi_bitmap_disabled [];
    // 'bluetooth', 8x8px
    static const unsigned char bluetooth_bitmap_bluetooth [];
    static const unsigned char remote_bitmap_remote [];


    uint8_t errType = 0;
    uint8_t wifiType = 0;
    uint8_t bluetoothType = 0;
    uint8_t remoteType = 0;
    //wifi
    void wifi_connect(const char* WIFI_SSID1, const char* WIFI_PASS1);
    void wifi_ap();
    void wifi_scan();
    void wifi_disable();
    //ir
    void ir_rx();
    void ir_tx();
    //mhz
    void mhz_tx();
    //other
    void getErr(const char* errMsg);
    void drawLines(const char* lineText);
    //gamegonki
    void gamegonki();

    // Variables
    static const char* autofliper;
    const char* lineText;
    uint8_t taskbar_show = 1;
  
    char UTC = 0; //your time zone (utc)
    

    int currentHours = 0;         // Поточні години
    int currentMinutes = 0;       // Поточні хвилини
    unsigned long lastSyncTime = 0; // Час останнього оновлення

    
  
   
    

private:
    // Private Members
    U8G2& _disp;   // Display object
    static const char* _BOOTSETItems[7];
    uint8_t _OKpin; // Pin for OK button
    uint8_t _OKsig; // Pin for OK button
    uint8_t _analogButton; // Pin for analog button
    const char* _ssidConfig; // SSID for Wi-Fi
    const char* _passwordConfig; // Wi-Fi password
    uint8_t _IR_tx; // IR TX pin
    uint8_t _IR_rx; // IR RX pin
    uint8_t _mHz_tx; // MHz TX pin
    uint8_t _vibroPin; // Vibration motor pin





    
   
    static uint8_t _BOOTSETPointer;

    // Static Methods
    void _BOOTSET();
};

#endif // FLEXIBLECLOCKLIBRARY_h
