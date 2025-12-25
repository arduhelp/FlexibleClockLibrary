 #include <U8g2lib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FlexibleClockLibrary.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#ifdef ESP32
 #include <NimBLEDevice.h>
 #include <IRremote.hpp>
#endif

NimBLEAdvertising *pAdvertising;
// Об'єкт дисплея U8G2 (для OLED дисплея I2C)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Налаштування Wi-Fi
const char* ssid = "YourSSID";
const char* password = "YourPassword";

// Ініціалізація об’єкта FlexibleClockLibrary
FlexibleClockLibrary clockLib(u8g2, 10, 0, A0, ssid, password, 5, 6, 7, 0);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
byte oksig = 0;
#define okpin 10
#define irtx 1
#define irrx 2

 
  uint16_t rawBuf[200];
  decode_type_t lastProtocol = UNKNOWN;  // Протокол останнього сигналу
  uint32_t lastData = 0;                 // Дані сигналу
  uint16_t rawLen = 0;                   // Довжина "сирих" даних, якщо треба


void scan_plus();   
void pmenuwifi();
void pmenuir();
void pmenumhz();
void pmenugames();
void pmenuscanplus();


char clockposX = 70;
char clockposY = 50;


// 'clock', 15x15px
const unsigned char item_bitmap_clock [] PROGMEM = {
  0xe0, 0x03, 0x18, 0x0c, 0x84, 0x10, 0x82, 0x20, 0x82, 0x20, 0x81, 0x40, 0x81, 0x40, 0x81, 0x47,
  0x01, 0x40, 0x01, 0x40, 0x02, 0x20, 0x02, 0x20, 0x04, 0x10, 0x18, 0x0c, 0xe0, 0x03
};
// 'games', 15x15px
const unsigned char item_bitmap_games [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x12, 0x24, 0xe1, 0x43, 0x1d, 0x48, 0x1d, 0x5c,
  0x1d, 0x48, 0x01, 0x40, 0xe1, 0x43, 0x11, 0x44, 0x11, 0x44, 0x0e, 0x38, 0x00, 0x00
};
// 'ir_tx', 15x15px
const unsigned char item_bitmap_ir_tx [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x04, 0x10, 0x02, 0x20, 0xf1, 0x47,
  0x08, 0x08, 0x04, 0x10, 0xe0, 0x03, 0xf0, 0x07, 0xf0, 0x07, 0xf0, 0x07, 0xf8, 0x0f
};
// 'ir_raw_rx', 15x15px
const unsigned char item_bitmap_ir_raw_rx [] PROGMEM = {
  0x08, 0x08, 0xf1, 0x47, 0x02, 0x20, 0x04, 0x10, 0xf8, 0x0f, 0x00, 0x00, 0x46, 0x22, 0xaa, 0x22,
  0xe6, 0x2a, 0xaa, 0x14, 0x00, 0x00, 0xe0, 0x03, 0xf0, 0x07, 0xf0, 0x07, 0xf8, 0x0f
};
// 'ir_rx', 15x15px
const unsigned char item_bitmap_ir_rx [] PROGMEM = {
  0x04, 0x10, 0x08, 0x08, 0xf1, 0x47, 0x02, 0x20, 0x04, 0x10, 0xf8, 0x0f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x03, 0xf0, 0x07, 0xf0, 0x07, 0xf0, 0x07, 0xf8, 0x0f
};
// 'mhz', 15x15px
const unsigned char item_bitmap_mhz [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x28, 0x0a, 0xa8, 0x0a, 0x28, 0x0a,
  0x88, 0x08, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'wifi_ap', 15x15px
const unsigned char item_bitmap_wifi_ap [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x18, 0xc6, 0x18, 0x00, 0x00, 0x00, 0x00,
  0xe0, 0x03, 0x10, 0x04, 0x10, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'wifi_disable', 15x15px
const unsigned char item_bitmap_wifi_disable [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00, 0xe8, 0x07, 0xd0, 0x0f, 0x2c, 0x1c, 0x4c, 0x18,
  0xe0, 0x03, 0x10, 0x05, 0x10, 0x06, 0x80, 0x04, 0x00, 0x08, 0x00, 0x10, 0x00, 0x00
};
// 'wifi_hack', 15x15px
const unsigned char item_bitmap_wifi_hack [] PROGMEM = {
  0x00, 0x00, 0x00, 0x0a, 0x00, 0x0a, 0x80, 0x20, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00,
  0x86, 0x01, 0x02, 0x01, 0x78, 0x00, 0x84, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'wifi', 15x15px
const unsigned char item_bitmap_wifi [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x07, 0xf8, 0x0f, 0x1c, 0x1c, 0x0c, 0x18,
  0xe0, 0x03, 0x10, 0x04, 0x10, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'wifi_scan', 15x15px
const unsigned char item_bitmap_wifi_scan [] PROGMEM = {
  0x00, 0x00, 0x00, 0x1c, 0x00, 0x22, 0x00, 0x51, 0x00, 0x49, 0x00, 0x41, 0x00, 0x22, 0xfc, 0x1c,
  0x86, 0x01, 0x02, 0x01, 0x78, 0x00, 0x84, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'exit_enter', 15x15px
const unsigned char item_bitmap_exit_enter [] PROGMEM = {
  0x00, 0x00, 0x00, 0x38, 0x00, 0x38, 0x00, 0x38, 0x60, 0x38, 0x70, 0x38, 0x38, 0x38, 0x1c, 0x38,
  0xfe, 0x3f, 0xff, 0x3f, 0xfe, 0x3f, 0x1c, 0x00, 0x38, 0x00, 0x70, 0x00, 0x60, 0x00
};

// 'settings', 15x15px
const unsigned char item_bitmap_settings [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x08, 0xc6, 0x0c, 0xee, 0x08, 0x7c, 0x08, 0x38, 0x1c,
  0x38, 0x1c, 0x38, 0x1c, 0x38, 0x1c, 0x38, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'server', 15x15px
const unsigned char item_bitmap_server [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x1f, 0x14, 0x11, 0x44, 0x10, 0xfc, 0x1f, 0x00, 0x00,
  0xfc, 0x1f, 0x04, 0x10, 0x54, 0x11, 0xfc, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'reboot', 15x15px
const unsigned char item_bitmap_reboot [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x10, 0x02, 0x08, 0x04, 0x04, 0x08, 0x04, 0x08, 0x04, 0x08,
  0x04, 0x08, 0x08, 0x02, 0x10, 0x04, 0xe0, 0x0f, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00
};
// 'ble', 15x15px
const unsigned char item_bitmap_ble [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x03, 0x20, 0x05, 0x40, 0x09, 0x80, 0x05, 0x00, 0x03,
  0x00, 0x03, 0x80, 0x05, 0x40, 0x09, 0x20, 0x05, 0x10, 0x03, 0x00, 0x01, 0x00, 0x00
};
// 'find_phone', 15x15px
const unsigned char item_bitmap_find_phone [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xf8, 0x0f, 0x08, 0x08, 0x0a, 0x28, 0x09, 0x48, 0x09, 0x48, 0x09, 0x48,
  0x09, 0x48, 0x09, 0x48, 0x09, 0x48, 0xca, 0x29, 0x08, 0x08, 0xf8, 0x0f, 0x00, 0x00
};
// 'image', 15x15px
const unsigned char item_bitmap_image [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x3f, 0x02, 0x20, 0x02, 0x22, 0x02, 0x27, 0x02, 0x22, 0x22, 0x20,
  0x72, 0x24, 0xfa, 0x2e, 0xfa, 0x2f, 0x02, 0x20, 0xfe, 0x3f, 0x00, 0x00, 0x00, 0x00
};



// 'frame', 120x24px
const unsigned char item_bitmap_frame [] PROGMEM = {
  0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x02,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f
};


// 'gear', 128x64px
const unsigned char wallpaper_bitmap_gear [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x40, 0x01, 0xb8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x60, 0xc1, 0x98, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0xe0, 0xe3, 0x05, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x20, 0x03, 0x04, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xfc, 0xff, 0xff, 0x7f, 0x07, 0x40, 0x98, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0c, 0x0f, 0x00, 0xc0, 0x0d, 0x20, 0xd8, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0c, 0x00, 0x00, 0x80, 0x10, 0x30, 0x98, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0e, 0x00, 0xc0, 0x80, 0x30, 0x10, 0x9c, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0xe0, 0xc3, 0x20, 0x3c, 0x84, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x10, 0x6c, 0x20, 0xe6, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x10, 0x38, 0xc2, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x30, 0x00, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x30, 0x00, 0x02, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x20, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x20, 0x00, 0x02, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x3e, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0xc0, 0x01, 0x00, 0x02, 0xc0, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x60, 0x00, 0x00, 0x03, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x20, 0x00, 0xe0, 0x3f, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x60, 0x00, 0x38, 0x62, 0x00, 0x20, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x40, 0x00, 0x0c, 0x02, 0x00, 0x30, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x80, 0x01, 0x86, 0x1f, 0x01, 0x1c, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x03, 0x63, 0x22, 0x03, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x02, 0x01, 0x42, 0x02, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x01, 0x02, 0x04, 0x06, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3e, 0x00, 0xb3, 0xc1, 0x8f, 0x24, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3c, 0xf8, 0xf0, 0xff, 0xff, 0xff, 0x18, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x2c, 0x1e, 0x80, 0x19, 0x82, 0x0c, 0x70, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x1f, 0x80, 0x01, 0x82, 0x0c, 0xc0, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x12, 0x00, 0x01, 0x02, 0x04, 0x40, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x32, 0x00, 0x11, 0x42, 0x04, 0x40, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0xe2, 0x03, 0x63, 0x22, 0x02, 0x60, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x80, 0x07, 0x86, 0x0f, 0x03, 0x3f, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x04, 0x0c, 0x82, 0x01, 0xe1, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0x04, 0x18, 0xe2, 0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x10, 0x04, 0x70, 0x3e, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x11, 0x06, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x01, 0x03, 0x00, 0x02, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x30, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x84, 0x01, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x00, 0xc1, 0x00, 0x02, 0x38, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x02, 0xff, 0x01, 0x02, 0xcc, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x02, 0x0e, 0x03, 0x02, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x02, 0x00, 0x03, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x02, 0x00, 0x83, 0x1f, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x02, 0x00, 0x63, 0x30, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x04, 0x06, 0x00, 0x23, 0x20, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0e, 0x07, 0x00, 0x32, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xfe, 0xff, 0xff, 0x1c, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x86, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1a, 0x87, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xfa, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1a, 0x80, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x80, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};



const char* MenuItems[9] = { "clock", "wifi", "IR", "MHz", "games", "settings" , "server", "info", "" }; // Ініціалізація статичного масиву
const char* WiFiMenuItems[7] = { "connect", "disable", "AP", "scan+", "prycoly", "back to menu" , "" };
const char* SettingsMenuItems[7] = { "GPIO", "SETclock", "find phone", "system info", "back to menu", "reboot" , "test" };
const char* IRMenuItems[7] = { "ir rx", "ir rx(raw)", "ir tx(raw)", "scan", "back to menu", "" ,"" };



void setup() {
  // Ініціалізація бібліотеки
  clockLib.begin();
  Serial.println("ready!");
  IrReceiver.begin(irrx, false); // false — без LED
  IrSender.begin(irtx);

  clockLib.taskbar_show = 1;
 // timeClient.begin(); // Ініціалізуємо NTP-клієнт
  clockLib.currentHours = 18;
  clockLib.currentMinutes = 58;
  clockLib.UTC = 1;
  clockLib.currentHours += clockLib.UTC;
  clockLib.ClockDisp(clockposX, clockposY,/*wallpaper_bitmap_bezmeteznist*/ wallpaper_bitmap_gear, 128, 64);

delay(100);
}


void loop() {

 // menu();
  updateClock();

clockLib.taskbar_draw(8);
  if (WiFi.status() != WL_CONNECTED && clockLib.wifiType != 0) {
    clockLib.wifiType = 3;
  }
  if (WiFi.status() == WL_CONNECTED) {
    clockLib.wifiType = 1;
  } 


  //u8g2.clearBuffer();

  //Serial.println(digitalRead(16));
  

  if (Serial.available()) {//debug
    String command = Serial.readStringUntil('\n');  // Зчитуємо команду до кінця рядка

    command.trim();  // Видаляємо зайві пробіли на початку та в кінці команди

    // Перевірка команд та виклик відповідних функцій
    if (command == "wifi_connect" || command == "wc") { //debug
      clockLib.wifi_connect(".exe", "./wificonnecting.exe");
    } else if (command == "wifi_ap" || command == "wa") {
      clockLib.wifi_ap("test_debug", "debugg");
    } else if (command == "wifi_scan" || command == "ws") {
      scan_plus();
    } else if (command == "wifi_disable" || command == "wd") {
      clockLib.wifi_disable();
    } else if (command == "ir_rx" || command == "irr") {
      clockLib.ir_rx();
    } else if (command == "ir_tx" || command == "irt") {
      clockLib.ir_tx();
    } else if (command == "mhz_tx" || command == "mhzt") {
      clockLib.mhz_tx();
    } else if (command == "getErr" || command == "gE") {
      clockLib.getErr("Sample Error Message");
    } else if (command == "drawLines" || command == "dL") {
      clockLib.drawLines("Sample Line Text");
    } else if (command == "gamegonki" || command == "gg") {
      clockLib.gamegonki();
    } else if (command == "ClockDisp" || command == "cd") {
      clockLib.ClockDisp(clockposX, clockposY,/*wallpaper_bitmap_bezmeteznist*/ wallpaper_bitmap_gear, 128, 64);
    } else if (command == "ClockUpdate" || command == "cu") {
      updateClock();
    } else if (command == "ClockUpdate1" || command == "cu1") {
      clockLib.ClockUpdate();
    } else if (command == "settings" || command == "ssg") {
      clockLib.ClockUpdate();
    } else if (command == "sourapple" || command == "sa") {
      sourapple();
    } else {
      Serial.println("Unknown command");
    }
  }

if (digitalRead(okpin) == oksig) {
      delay(1000); 
      if (digitalRead(okpin) == oksig) { 
       menu(); }}



      u8g2.clearBuffer();
    u8g2.drawStr(10, 20, "load... debug...");
    u8g2.drawStr(10, 30, "hold OK for EXIT.");
    u8g2.sendBuffer(); 
       delay(100);



}

//AI code
void updateClock() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long localTime = 0;
    unsigned long lastMillis = 0;
    timeClient.setTimeOffset(clockLib.UTC * 3600); // Налаштування UTC
    timeClient.update();
    localTime = timeClient.getEpochTime();
    lastMillis = millis();


    unsigned long adjustedTime = localTime + (clockLib.UTC * 3600);
    clockLib.currentHours = (adjustedTime % 86400L) / 3600;
    clockLib.currentMinutes = (adjustedTime % 3600) / 60;
  }
}
//end AI code

  static int parampam = 0; // Змінна повинна бути статичною, щоб зберігати значення між викликами функції
  static unsigned long lastTime = 0; // Змінна для таймера автофліппера
void menu() { // menu
 for (int i = 0; i = 20; i++) {
  clockLib.taskbar_draw(8);
  u8g2.setFont(u8g2_font_crox1hb_tf);
  if (WiFi.status() != WL_CONNECTED && clockLib.wifiType != 0) {
    clockLib.wifiType = 3;
  }
  if (WiFi.status() == WL_CONNECTED) {
    clockLib.wifiType = 1;
  }
  
   

  // Виведення пунктів меню
  switch (parampam) {
    case 0: // Clock
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_clock);
      u8g2.drawStr(30, 22, MenuItems[7]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        clockLib.ClockDisp(clockposX, clockposY,/*wallpaper_bitmap_bezmeteznist*/ wallpaper_bitmap_gear, 128, 64);
      }
      //u8g2.sendBuffer(); 60 30
      break;

    case 1: // WiFi
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_wifi);
      u8g2.drawStr(30, 22, MenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        delay(1000);
        pmenuwifi();
      }
      //u8g2.sendBuffer();
      break;

    case 2: // IR
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_ir_tx);
      u8g2.drawStr(30, 22, MenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        pmenuir();
      }
      //u8g2.sendBuffer();
      break;

    case 3: // MHz
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_mhz);
      u8g2.drawStr(30, 22, MenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        clockLib.getErr("there is no GUI");
      }
      //u8g2.sendBuffer();
      break;

    case 4: // Game
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_games);
      u8g2.drawStr(30, 22, MenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        clockLib.gamegonki();
      }
      //u8g2.sendBuffer();
      break;

    case 5: //settings
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_settings);
      u8g2.drawStr(30, 22, MenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        pmenusettings();
      }
      //u8g2.sendBuffer();
      break;

    case 6: // server
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_server);
      u8g2.drawStr(30, 22, MenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, MenuItems[parampam]);
      u8g2.drawStr(30, 60, MenuItems[0]);
      if (digitalRead(okpin) == oksig) {
        clockLib.getErr("there is no GUI");
      }
      //u8g2.sendBuffer();
      break;

    default:
      parampam = 0; // Повертаємось до першого пункту, якщо вийшли за межі
      break;
  }
  u8g2.drawStr(126, parampam * 8, ".");

  // Автоматичний фліп
  if (clockLib.autofliper == 1) {
    if (millis() - lastTime >= 1000) { // Якщо пройшла 1 секунда
      lastTime = millis(); // Оновлюємо таймер
      u8g2.clearBuffer();
      parampam++; // Переходимо до наступного пункту
      u8g2.sendBuffer();
      if (parampam > 6) { // Якщо перевищили кількість пунктів меню
        parampam = 0; // Повертаємось до першого пункту
      }
    }
  }
  u8g2.sendBuffer(); // Оновлюємо екран
  delay(30);
  
 }
 return;
}
//----------------------
//----settings-menu-----
//----------------------
//{ "wallpaper", "SETclock", "find phone","system info", "back to menu", "reboot" ,"test" };
void pmenusettings() { // pmenusettings
    u8g2.clearBuffer();
    u8g2.drawStr(30, 40, "settings");
    u8g2.sendBuffer();
    delay(1000);
 for (int i = 0; i = 20; i++) {
      clockLib.taskbar_draw(8);
      u8g2.setFont(u8g2_font_crox1hb_tf);
     // u8g2.setFont(u8g2_font_crox1hb_tf);
  

  

  // Виведення пунктів меню
  switch (parampam) {
    case 0: // walpaper
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_image);
      u8g2.drawStr(30, 22, SettingsMenuItems[7]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {}
      //u8g2.sendBuffer(); 60 30
      break;

    case 1: // setclock
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_clock);
      u8g2.drawStr(30, 22, SettingsMenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {}
      //u8g2.sendBuffer();
      break;

    case 2: // find phone
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_find_phone);
      u8g2.drawStr(30, 22, SettingsMenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        clockLib.buzzertest();
      }
      //u8g2.sendBuffer();
      break;

    case 3: // sys inf
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, clockLib.def_bitmap_def_unknown);
      u8g2.drawStr(30, 22, SettingsMenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        clockLib.getErr("there is no GUI");
      }
      //u8g2.sendBuffer();
      break;

    case 4: //back
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_exit_enter);
      u8g2.drawStr(30, 22, SettingsMenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        delay(700);
        return;
      }
      //u8g2.sendBuffer();
      break;

    case 5: //reboot
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_reboot);
      u8g2.drawStr(30, 22, SettingsMenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[parampam + 1]);
      if (digitalRead(okpin) == oksig) {
        for (int i = 0; i = 2000; i++) {
          u8g2.clearBuffer();
          u8g2.drawStr(30, 40, "rebooting");
          u8g2.sendBuffer();}
      }
      //u8g2.sendBuffer();
      break;

    case 6: // test
      u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
      u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_games);
      u8g2.drawStr(30, 22, SettingsMenuItems[parampam - 1]);
      u8g2.drawStr(30, 40, SettingsMenuItems[parampam]);
      u8g2.drawStr(30, 60, SettingsMenuItems[0]);
      if (digitalRead(okpin) == oksig) {
        clockLib.getErr("there is no GUI");
      }
      //u8g2.sendBuffer();
      break;

    default:
      parampam = 0; // Повертаємось до першого пункту, якщо вийшли за межі
      break;
  }
  u8g2.drawStr(126, parampam * 8, ".");

  // Автоматичний фліп
  if (clockLib.autofliper == 1) {
    if (millis() - lastTime >= 1000) { // Якщо пройшла 1 секунда
      lastTime = millis(); // Оновлюємо таймер
      u8g2.clearBuffer();
      parampam++; // Переходимо до наступного пункту
      u8g2.sendBuffer();
      if (parampam > 6) { // Якщо перевищили кількість пунктів меню
        parampam = 0; // Повертаємось до першого пункту
      }
    }
  }
  u8g2.sendBuffer(); // Оновлюємо екран
      delay(20);
 }
return;
     
  
}

  //----------------------
  //------wifi-menu-------
  //----------------------
  void pmenuwifi() { // menu
    u8g2.clearBuffer();
    u8g2.drawStr(30, 40, "wifi");
    u8g2.sendBuffer();
    delay(1000);
    for (int i = 0; i = 20; i++) {
      clockLib.taskbar_draw(8);
      u8g2.setFont(u8g2_font_crox1hb_tf);
      

      //u8g2.clearBuffer(); // Очищення буфера дисплея

      // Виведення пунктів меню
      switch (parampam) {//{ "connect", "disable", "AP","scan", "prycoly", "" ,"" };
        case 0: // connect
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_wifi);
          u8g2.drawStr(30, 22, WiFiMenuItems[5]);
          u8g2.drawStr(30, 40, WiFiMenuItems[parampam]);
          u8g2.drawStr(30, 60, WiFiMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            clockLib.wifi_connect(".exe", "./wificonnecting.exe");
          }
          //u8g2.sendBuffer();
          break;

        case 1: // disable
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_wifi_disable);
          u8g2.drawStr(30, 22, WiFiMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, WiFiMenuItems[parampam]);
          u8g2.drawStr(30, 60, WiFiMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            clockLib.wifi_disable();
          }
          //u8g2.sendBuffer();
          break;

        case 2: // AP
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_wifi_ap);
          u8g2.drawStr(30, 22, WiFiMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, WiFiMenuItems[parampam]);
          u8g2.drawStr(30, 60, WiFiMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            clockLib.wifi_ap("Wemos", "1243qwertyuiop");
          }
          //u8g2.sendBuffer();
          break;

        case 3: // scan
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_wifi_scan);
          u8g2.drawStr(30, 22, WiFiMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, WiFiMenuItems[parampam]);
          u8g2.drawStr(30, 60, WiFiMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            scan_plus();
          }
          //u8g2.sendBuffer();
          break;

        case 4: // prycoly
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_wifi_hack);
          u8g2.drawStr(30, 22, WiFiMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, WiFiMenuItems[parampam]);
          u8g2.drawStr(30, 60, WiFiMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            sourapple();
            }
          //u8g2.sendBuffer();
          break;

        case 5: // exit
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_exit_enter);
          u8g2.drawStr(30, 22, WiFiMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, WiFiMenuItems[parampam]);
          u8g2.drawStr(30, 60, WiFiMenuItems[0]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            return;
          }
          //u8g2.sendBuffer();
          break;

        default:
          parampam = 0; // Повертаємось до першого пункту, якщо вийшли за межі
          break;
      }
      u8g2.drawStr(126, parampam * 8, ".");

      // Автоматичний фліп
      if (clockLib.autofliper == 1) {
        if (millis() - lastTime >= 1000) { // Якщо пройшла 1 секунда
          lastTime = millis(); // Оновлюємо таймер
          u8g2.clearBuffer();
          parampam++; // Переходимо до наступного пункту
          u8g2.sendBuffer();
          if (parampam > 5) { // Якщо перевищили кількість пунктів меню
            parampam = 0; // Повертаємось до першого пункту
          }
        }
      }

      u8g2.sendBuffer(); // Оновлюємо екран
      delay(20);
    }
    return;
  }

  void pmenuir() {
     u8g2.clearBuffer();
    u8g2.drawStr(30, 40, "ir menu");
    u8g2.sendBuffer();
    delay(1000);
    for (int i = 0; i = 20; i++) {
      clockLib.taskbar_draw(8);
      u8g2.setFont(u8g2_font_crox1hb_tf);
      

      //u8g2.clearBuffer(); // Очищення буфера дисплея

      // Виведення пунктів меню
      switch (parampam) {//{ "ir rx", "ir rx(raw)", "ir tx(raw)", "scan", "exit", "" ,"" };
        case 0: // rx
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_ir_rx);
          u8g2.drawStr(30, 22, IRMenuItems[5]);
          u8g2.drawStr(30, 40, IRMenuItems[parampam]);
          u8g2.drawStr(30, 60, IRMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            irrxF();
          }
          //u8g2.sendBuffer();
          break;

        case 1: // rx raw
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_ir_raw_rx);
          u8g2.drawStr(30, 22, IRMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, IRMenuItems[parampam]);
          u8g2.drawStr(30, 60, IRMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            irrxRawF();
          }
          //u8g2.sendBuffer();
          break;

        case 2: // tx
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_ir_raw_rx);
          u8g2.drawStr(30, 22, IRMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, IRMenuItems[parampam]);
          u8g2.drawStr(30, 60, IRMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            irtxF();
          }
          //u8g2.sendBuffer();
          break;

        case 3: // scan
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_ir_tx);
          u8g2.drawStr(30, 22, IRMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, IRMenuItems[parampam]);
          u8g2.drawStr(30, 60, IRMenuItems[parampam + 1]);
          if (digitalRead(okpin) == oksig) {
            //scan_plus();
          }
          //u8g2.sendBuffer();
          break;

        case 4: // exit
          u8g2.drawXBMP(0, 25, 120, 24, item_bitmap_frame);
          u8g2.drawXBMP(5, 28, 15, 15, item_bitmap_exit_enter);
          u8g2.drawStr(30, 22, IRMenuItems[parampam - 1]);
          u8g2.drawStr(30, 40, IRMenuItems[parampam]);
          u8g2.drawStr(30, 60, IRMenuItems[0]);
          if (digitalRead(okpin) == oksig) {
            delay(1000);
            return;
          }
          //u8g2.sendBuffer();
          break;

        default:
          parampam = 0; // Повертаємось до першого пункту, якщо вийшли за межі
          break;
      }
      u8g2.drawStr(126, parampam * 8, ".");

      // Автоматичний фліп
      if (clockLib.autofliper == 1) {
        if (millis() - lastTime >= 1000) { // Якщо пройшла 1 секунда
          lastTime = millis(); // Оновлюємо таймер
          u8g2.clearBuffer();
          parampam++; // Переходимо до наступного пункту
          u8g2.sendBuffer();
          if (parampam > 4) { // Якщо перевищили кількість пунктів меню
            parampam = 0; // Повертаємось до першого пункту
          }
        }
      }

      u8g2.sendBuffer(); // Оновлюємо екран
      delay(20);
    }
    return;
   }
  void pmenumhz() {}
  void pmenugames() {}











  //other-------------------------------

void irrxF() {
  while(true){
    delay(10);
    if (IrReceiver.decode()) {

      uint32_t irCode = IrReceiver.decodedIRData.decodedRawData;

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_6x10_tf);

      u8g2.drawStr(0, 12, "IR code:");
      char buf[20];
      sprintf(buf, "0x%08lX", irCode);
      u8g2.drawStr(0, 28, buf);
      u8g2.sendBuffer();
      IrReceiver.resume();
     }
     if (digitalRead(okpin) == oksig) {
      // очищаємо повідомлення
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      return;
    }
    
    }
  }
//--- ai code ---
void irrxRawF() {
  // Ініціалізуємо приймач (не зашкодить, навіть якщо вже ініціалізовано)
 // IrReceiver.begin();
  u8g2.setFont(u8g2_font_6x10_tf);

  // Екран: очікуємо сигнал
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "IR receive - waiting...");
  u8g2.drawStr(0, 28, "Press OK to exit");
  u8g2.sendBuffer();

  // цикл прийому
  while (true) {
    // вихід по кнопці OK
    if (digitalRead(okpin) == oksig) {
      // очищаємо повідомлення
      u8g2.clearBuffer();
      u8g2.sendBuffer();
      return;
    }

    // якщо прийшов код
    if (IrReceiver.decode()) {
      // зберігаємо базову інформацію
      auto &d = IrReceiver.decodedIRData;
      lastProtocol = d.protocol;                // enum/protocol
      lastData = d.decodedRawData;                        // 32-bit дані (команда/код)
      rawLen = 0;                                         // якщо не маємо raw, лишаємо 0

      // Спроба зчитати сирі імпульси (якщо вони є / бібліотека дозволяє)
      // Нотатка: API бібліотек IR може відрізнятися; якщо у тебе є доступ до сирих даних,
      // сюди можна скопіювати їх у rawBuf[] і задати rawLen.
      // Наприклад (псевдокод, працюватиме не у всіх версіях):
      // if (d.protocol == UNKNOWN && d.numberOfBits==0) { copy raw pulses... rawLen = ... }

      // Формуємо рядок для відображення
      char buf[64];
      // показ назви протоколу (людський текст)
      const char *pname = "UNKNOWN";
      switch (d.protocol) {
        case NEC:     pname = "NEC"; break;
        case SONY:    pname = "SONY"; break;
        case RC5:     pname = "RC5"; break;
        case RC6:     pname = "RC6"; break;
        case SAMSUNG: pname = "SAMSUNG"; break;
        case LG:      pname = "LG"; break;
        // додай інші протоколи, якщо потрібно
        default:      pname = "OTHER/RAW"; break;
      }

      // Відображення на дисплеї
      u8g2.clearBuffer();
      u8g2.setCursor(0, 12);
      u8g2.print("Protocol: ");
      u8g2.print(pname);

      u8g2.setCursor(0, 28);
      snprintf(buf, sizeof(buf), "Data: 0x%08lX", (unsigned long)lastData);
      u8g2.print(buf);

      // Якщо є сирі дані, відобразимо кількість
      if (rawLen > 0) {
        snprintf(buf, sizeof(buf), "Raw len: %d", rawLen);
        u8g2.setCursor(0, 44);
        u8g2.print(buf);
      } else {
        u8g2.setCursor(0, 44);
        u8g2.print("Raw: none");
      }

      u8g2.sendBuffer();

      // Збережено — готуємось до наступного сигналу
      IrReceiver.resume();
      delay(80); // короткий інтервал, щоб не дублювати (не обов'язково)
    } // if decode

    // трохи "полегшимо" цикл
    delay(5);
  } // while
}

void irtxF() {
  byte sended = false;
  // перевірка кнопки OK: якщо натиснута — не надсилати
  if (digitalRead(okpin) == oksig) return;

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Sending...");
  u8g2.sendBuffer();
  if(sended == false){
  // якщо немає даних
  if (lastProtocol == 0 && lastData == 0 && rawLen == 0) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 12, "No data stored");
    u8g2.sendBuffer();
    delay(300);
    return;
  }

  // Спроба надіслати згідно з protocol
  switch (lastProtocol) {
    case NEC:
     // IrSender.begin();
      IrSender.sendNEC(lastData, 32);
      sended = true;
      break;
    case SONY:
     // IrSender.begin();
      // у Sony зазвичай 12 або 15 біт — бібліотека інформує по кількості бітів, але ми використовуємо 12 як у твому прикладі
      IrSender.sendSony(lastData, 12);
      sended = true;
      break;
    case SAMSUNG:
     // IrSender.begin();
      // Багато реалізацій Samsung використовують 32 біта
     // IrSender.sendSamsung(lastData, 32);
      break;
    case LG:
     // IrSender.begin();
      IrSender.sendLG(lastData, 28);
      sended = true;
      break;
    case RC5:
     // IrSender.begin();
      IrSender.sendRC5(lastData, 12);
      sended = true;
      break;
    case RC6:
     // IrSender.begin();
      IrSender.sendRC6(lastData, 20);
      sended = true;
      break;
    default:
      // Якщо маємо сирі імпульси, можна відтворити їх вручну (необхідно мати rawBuf і rawLen)
      if (rawLen > 0) {
     //   IrSender.begin();
        // приклад: IrSender.sendRaw(rawBuf, rawLen, frequency);
        // Але точний виклик залежить від версії бібліотеки IrSender.
        // Якщо в тебе є rawBuf[] та частота, тут можна викликати sendRaw.
      } else {
        u8g2.clearBuffer();
        u8g2.drawStr(0, 12, "Unsupported proto");
        u8g2.sendBuffer();
        return;
      }
  }
}else{return;}
  // Підтвердження
  u8g2.clearBuffer();
  u8g2.drawStr(0, 12, "Sent!");
  u8g2.sendBuffer();
  delay(120);
}




//--- end ai code ---

  
  void pmenuscanplus() {
    u8g2.setCursor(0, 52);
    u8g2.print("rescan/exit");
    u8g2.sendBuffer();
    delay(200);
  }
  void scan_plus() {
  u8g2.setFont(u8g2_font_6x10_tf);
  int currentNetwork = 0;
  int numNetworks = 0;
  bool buttonPressed = false;
  unsigned long buttonPressTime = 0;

  while (true) {
    // Ініціалізація дисплея та WiFi
    if (numNetworks == 0) {
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      u8g2.clearBuffer();
      u8g2.drawStr(0, 20, "Scanning...");
      u8g2.sendBuffer();

      numNetworks = WiFi.scanNetworks();
      delay(500);
    }

    // кнопка OK
    if (digitalRead(okpin) == oksig) {
      if (!buttonPressed) {
        buttonPressed = true;
        buttonPressTime = millis();
      } else if (millis() - buttonPressTime > 1000) { // довге натискання
        buttonPressed = false;
        return;
      }
    } else {
      if (buttonPressed && millis() - buttonPressTime <= 1000) { // коротке натискання
        if (numNetworks > 0) {
          currentNetwork = (currentNetwork + 1) % numNetworks;
        } else {
          numNetworks = WiFi.scanNetworks();
        }
      }
      buttonPressed = false;
    }

    // вивід на OLED
    u8g2.clearBuffer();

    if (numNetworks > 0) {
      u8g2.setCursor(0, 15);
      u8g2.print("SSID:");
      u8g2.print(WiFi.SSID(currentNetwork));

      // ==== MAC адреса ====
      #ifdef ESP8266
        u8g2.setCursor(0, 25);
        u8g2.print("MAC: ");
        u8g2.print(WiFi.BSSIDstr(currentNetwork));
      #else
        uint8_t *bssid = WiFi.BSSID(currentNetwork);
        char macStr[18];
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
                bssid[0], bssid[1], bssid[2],
                bssid[3], bssid[4], bssid[5]);
        u8g2.setCursor(0, 25);
        u8g2.print("MAC:");
        u8g2.print(macStr);
      #endif

      u8g2.setCursor(0, 35);
      u8g2.print("RSSI: ");
      u8g2.print(WiFi.RSSI(currentNetwork));

      u8g2.setCursor(0, 45);
      u8g2.print("Ch: ");
      u8g2.print(WiFi.channel(currentNetwork));

      u8g2.setCursor(0, 55);
      u8g2.print("Sec: ");
      u8g2.print(getEncryptionType(WiFi.encryptionType(currentNetwork)).c_str());

      // сторінка
      u8g2.setCursor(70, 62);
      u8g2.print("page ");
      u8g2.print(currentNetwork);
      u8g2.print("/");
      u8g2.print(numNetworks - 1);
    } else {
      u8g2.drawStr(0, 20, "No networks found.");
    }

    u8g2.sendBuffer();
    delay(100);
  }
}

// === універсальна функція безпеки ===
String getEncryptionType(int type) {
  #ifdef ESP8266
    switch (type) {
      case ENC_TYPE_NONE: return "Open";
      case ENC_TYPE_WEP: return "WEP";
      case ENC_TYPE_TKIP: return "WPA/TKIP";
      case ENC_TYPE_CCMP: return "WPA2/CCMP";
      case ENC_TYPE_AUTO: return "Auto";
      default: return "Unknown";
    }
  #else // ESP32
    switch ((wifi_auth_mode_t)type) {
      case WIFI_AUTH_OPEN: return "Open";
      case WIFI_AUTH_WEP: return "WEP";
      case WIFI_AUTH_WPA_PSK: return "WPA-PSK";
      case WIFI_AUTH_WPA2_PSK: return "WPA2-PSK";
      case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
      case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ENT";
      case WIFI_AUTH_WPA3_PSK: return "WPA3-PSK";
      case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/WPA3";
      default: return "Unknown";
    }
  #endif
}

NimBLEAdvertisementData getOAdvertisementData() {
  NimBLEAdvertisementData randomAdvertisementData = NimBLEAdvertisementData();
  uint8_t packet[17];
  uint8_t i = 0;

  packet[i++] = 16;    // Packet Length
  packet[i++] = 0xFF;  // Manufacturer Specific
  packet[i++] = 0x4C;  // Apple Inc.
  packet[i++] = 0x00;  
  packet[i++] = 0x0F;  
  packet[i++] = 0x05;  
  packet[i++] = 0xC1;  
  const uint8_t types[] = { 0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0 };
  packet[i++] = types[rand() % sizeof(types)];
  esp_fill_random(&packet[i], 3); // Authentication Tag
  i += 3;   
  packet[i++] = 0x00;
  packet[i++] = 0x00;
  packet[i++] = 0x10;
  esp_fill_random(&packet[i], 3);

  randomAdvertisementData.addData(packet, 17);
  return randomAdvertisementData;
}
void sourapple() {
  delay(100);
  NimBLEDevice::init("");
  delay(500);
  u8g2.setCursor(0, 52);
  u8g2.print("sourapple");
  u8g2.sendBuffer();
  delay(200);

  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN, ESP_PWR_LVL_P9);

  NimBLEServer *pServer = NimBLEDevice::createServer();
  pAdvertising = pServer->getAdvertising();

  // Безкінечний цикл
  while(true) {
    NimBLEAdvertisementData advertisementData = getOAdvertisementData();
    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->start();
    delay(20);
    pAdvertising->stop();
    delay(40);
   if (digitalRead(okpin) == oksig) {
      pAdvertising->stop();  // зупинити рекламу
      delay(20);
      return; // вихід з функції назад у setup()
    }
  }
}
