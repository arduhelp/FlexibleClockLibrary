#include "FlexibleClockLibrary.h"
#include <Arduino.h>
#include <U8g2lib.h>


 const unsigned char FlexibleClockLibrary::_err_bitmap_err [] PROGMEM = {
    0x00, 0x00, 0x10, 0x28, 0x6c, 0x7c, 0xee, 0xfe
};

const unsigned char FlexibleClockLibrary::_err_bitmap_noerr [] PROGMEM = {
    0x00, 0x00, 0x00, 0x80, 0x40, 0x22, 0x14, 0x08
};

const unsigned char FlexibleClockLibrary::_err_bitmap_qestoin [] PROGMEM = {
    0x00, 0x00, 0x7f, 0x73, 0x6f, 0x67, 0x7f, 0x77
};
const unsigned char FlexibleClockLibrary::_err_bitmap_loading [] PROGMEM = {
	0x00, 0x10, 0x54, 0x00, 0xc6, 0x00, 0x54, 0x10
};
// 'def_unknown', 8x8px
const unsigned char FlexibleClockLibrary::def_bitmap_def_unknown [] PROGMEM = {
	0x3e, 0x4a, 0x52, 0x62, 0x42, 0x42, 0x42, 0x7e
};
// 'ap', 8x8px
const unsigned char FlexibleClockLibrary::wifi_bitmap_ap [] PROGMEM = {
	0x00, 0x00, 0x00, 0x92, 0x00, 0x38, 0x44, 0x10
};
// 'nothing', 8x8px
const unsigned char FlexibleClockLibrary::wifi_bitmap_nothing [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};
// 'wifi', 8x8px
const unsigned char FlexibleClockLibrary::wifi_bitmap_wifi [] PROGMEM = {
	 0x00, 0x00, 0x00, 0x3e, 0x41, 0x1c, 0x22, 0x08
};
const unsigned char FlexibleClockLibrary::wifi_bitmap_disabled [] PROGMEM = {
	0x00, 0x00, 0x01, 0x3a, 0x45, 0x08, 0x12, 0x28
};
// 'bluetooth', 8x8px
const unsigned char FlexibleClockLibrary::bluetooth_bitmap_bluetooth [] PROGMEM = {
	0x30, 0x52, 0x54, 0x38, 0x38, 0x54, 0x52, 0x30
};
const unsigned char FlexibleClockLibrary::remote_bitmap_remote [] PROGMEM = {
	0x00, 0x00, 0x00, 0x7e, 0x42, 0x52, 0x4a, 0x42
};

FlexibleClockLibrary::FlexibleClockLibrary(U8G2& disp, uint8_t OKpin, uint8_t OKsig, uint8_t analogButton,
                                           const char* ssidConfig, const char* passwordConfig,
                                           uint8_t IR_tx, uint8_t IR_rx, uint8_t mHz_tx, uint8_t buzzerPin)
    : _disp(disp), _OKpin(OKpin), _OKsig(OKsig), _analogButton(analogButton), _ssidConfig(ssidConfig), 
      _passwordConfig(passwordConfig), _IR_tx(IR_tx), _IR_rx(IR_rx), 
      _mHz_tx(mHz_tx), _buzzerPin(buzzerPin), server(80) {
    // Ініціалізація змінних класу
}


unsigned long cur_milllis = 0;
unsigned long last_tick = 0;

unsigned int lhour = 0;
unsigned int lmin = 0;
unsigned int lsec = 0;


const char* FlexibleClockLibrary::_BOOTSETItems[7] = { "autofliper", "OTA update", "Option3","utc+", "Exit", "info" ,"" }; // Ініціалізація статичного масиву
    const char* FlexibleClockLibrary::_host_ota = "espfcl-webupdate";
    const char* FlexibleClockLibrary::_ssid_ota = "espFCL-OTA";
    const char* FlexibleClockLibrary::_password_ota = "098765432123";
    //----50%-AI-code------------------
    const char* FlexibleClockLibrary::_serverIndex_ota = "<h2 style='margin-bottom: 20px; font-weight: 700; color: #333;'>FlexibleClockLib Firmware Update</h2><form method='POST' action='/update' enctype='multipart/form-data' style='display: inline-block; background-color: #1e1e1e; padding: 20px; border-radius: 8px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.5);'><input type='file' name='update' accept='.bin' style='display: block; width: 100%; padding: 12px; margin-bottom: 15px; border: 1px solid #444; border-radius: 4px; font-size: 14px; background: linear-gradient(135deg, #2b2b2b, #3c3c3c); color: #e0e0e0; outline: none; cursor: pointer; box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.3);'><input type='submit' value='Update' style='display: block; width: 100%; padding: 12px; border: none; border-radius: 4px; background: linear-gradient(135deg, #fc4a1a, #f7b733); color: #fff; font-size: 16px; font-weight: 500; text-transform: uppercase; font-weight: bold; cursor: pointer; transition: background 0.3s; box-shadow: 0 3px 6px rgba(0, 0, 0, 0.3);'></form>";
    //---end-50%-AI-code---------------

int FlexibleClockLibrary::autofliper = 1;


//-------------------------------------
//-------------begin-------------------
//-------------------------------------
void FlexibleClockLibrary::begin() {
    pinMode(_OKpin, INPUT_PULLUP); // OK pin
    // attachInterrupt(digitalPinToInterrupt(_OKpin), handleOkInterrupt, FALLING);
    Serial.begin(115200);
    Serial.println("loading... serial");
    _disp.begin();
    Serial.println("loading... display");
    #ifdef ESP32
    //IrSender.begin(_IR_tx);
    //IrReceiver.begin(_IR_rx, ENABLE_LED_FEEDBACK);
    Serial.println("loading... ir");
    #endif
    clearDisp();
    _disp.setFont(u8g2_font_6x10_tf);
    Serial.print(digitalRead(_OKpin));
    uint8_t taskbar_show = 1;
    
    wifiType = 3;
    _disp.clearBuffer(); 
     _disp.setFont(u8g2_font_6x10_tf);
     _disp.drawStr(5, 54, "FlexibleClockLib"); 
     _disp.drawStr(80, 64, "v1.1s9"); 
     int ClockUpdateMillis = 1;
     _disp.sendBuffer();
     delay(1000);
     if(digitalRead(_OKpin) == _OKsig) {
        _BOOTSET();
     }
     delay(1000);
     _disp.clearBuffer(); // Очищаємо буфер дисплея
     _disp.sendBuffer();  // Виводимо змінений (порожній) буфер на екран
        
}




//------------------------------------- ┳┓ ┏┓ ┏┓ ┏┳┓ ┏┓ ┏┓ ┏┳┓
//-----BOOTSET------------------------- ┣┫ ┃┃ ┃┃  ┃  ┗┓ ┣   ┃ 
//------------------------------------- ┻┛ ┗┛ ┗┛  ┻  ┗┛ ┗┛  ┻ 


// BOOTSETTINGS
uint8_t FlexibleClockLibrary::_BOOTSETPointer = 1;

void FlexibleClockLibrary::_BOOTSET() { clearDisp();
    unsigned long lastTime = 0;
    delay(5000);
    _disp.setFont(u8g2_font_6x10_tf);
 for (int loopboot = 0; loopboot < 30000; loopboot++) { 
   // clearDisp();
    
    Serial.println("BOOTSET!");
    Serial.println("OK pin" + digitalRead(_OKpin));

    int lineHeight = 11;
    int y = 11;  // Fixed variable for drawing the menu
    uint8_t yP = _BOOTSETPointer*11;
//yP позиція, _BOOTSETPointer номер пункту

//delay(1000);

    if (millis() - lastTime >= 1000) { // Якщо пройшла 1 секунда
    lastTime = millis(); // Оновлюємо час
    _disp.clearBuffer();
    _BOOTSETPointer++;
   
  }

 
// delay(200);

    
    
    //вивід на дисплей пункти
    for (int i = 0; i < 7; i++) { // Use 3 items instead of 5
        _disp.drawStr(10, y, _BOOTSETItems[i]);  // Виводимо кожен пункт меню
        y += lineHeight;  // Переходимо до наступного рядка
         _disp.drawStr(0, yP - lineHeight, " ");
         _disp.drawStr(0, yP, ">"); 
         _disp.drawStr(80, 11, String(autofliper).c_str()); 
    }
   if(_BOOTSETPointer == 7){_BOOTSETPointer = 0;}
        if(digitalRead(_OKpin) == _OKsig){
            switch(_BOOTSETPointer){
                case 1: if(autofliper == 1){autofliper = 0;}else{autofliper = 1;} delay(50); break;
                case 2: _OTA_UPDATE(); delay(50); break;

                case 5:  _disp.clearBuffer(); _disp.drawStr(0, 20, "3sec.. reboot");  _disp.sendBuffer(); delay(3000); ESP.restart(); break;
                case 6:  _disp.clearBuffer(); _disp.drawStr(0, 10, "powered by"); _disp.drawStr(0, 20, "FlexibleClockLibrary"); _disp.drawStr(0, 40, "github.com/arduhelp"); _disp.drawStr(0, 50, "/FlexibleClockLibrary"); _disp.sendBuffer(); delay(20000); break;
            }
        }

  
    
    
    _disp.sendBuffer();  // Оновлюємо екран
   // delay(2000);
 }
}
//-------------OTA-update--------------
void FlexibleClockLibrary::_OTA_UPDATE() {
   _disp.clearBuffer();
   _disp.sendBuffer();
   _disp.drawStr(0, 10, "warning! OTA update");
   _disp.drawStr(0, 30, "hold OK for exit");
   _disp.sendBuffer();
   delay(2000);
       if(digitalRead(_OKpin) == _OKsig){return;}
   _disp.clearBuffer();
   _disp.sendBuffer();
   delay(2000);
//--------AI-code----------------------
    const int PASSWORD_LENGTH = 9;
    const char charset[] = "123456789abcdef";
    const int charsetSize = sizeof(charset) - 1;
      char _password_ota1[PASSWORD_LENGTH + 1];
      for (int i = 0; i < PASSWORD_LENGTH; i++) {
        _password_ota1[i] = charset[random(0, charsetSize)];
      }
      _password_ota1[PASSWORD_LENGTH] = '\0';
      _password_ota = strdup(_password_ota1);
//---------end-AI-code-----------------
    Serial.println(_password_ota);
    _disp.drawStr(0, 10, "create AP please");
    _disp.drawStr(0, 22, "ssid:");
    _disp.drawStr(30, 22, _ssid_ota);
    _disp.drawStr(0, 33, "pswd:");
    _disp.drawStr(30, 33, _password_ota);
    _disp.sendBuffer();
    delay(30000);
    _disp.clearBuffer();
    _disp.sendBuffer();

  Serial.println();
  Serial.println("Booting Sketch...");

  _disp.drawStr(0, 10, "Booting Sketch...");
  _disp.sendBuffer();

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(_ssid_ota, _password_ota);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    MDNS.begin(_host_ota);
    server.on("/", HTTP_GET, [this]() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", _serverIndex_ota);
    });
    server.on(
      "/update", HTTP_POST, [this]() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
      },
      [this]() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Serial.setDebugOutput(true);
          #ifdef ESP8266
           WiFiUDP::stopAll();
          #endif

          Serial.printf("Update: %s\n", upload.filename.c_str());
            _disp.clearBuffer();
            _disp.sendBuffer();
            _disp.drawStr(0, 22, "Update...");
            _disp.sendBuffer();
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          if (!Update.begin(maxSketchSpace)) {  // start with max available size
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {  // true to set the size to the current progress
              _disp.drawStr(0, 33, "Update Success:Rebooting...");
              _disp.sendBuffer();
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
        yield();
      });
    server.begin();
    MDNS.addService("http", "tcp", 80);
    Serial.print("Device IP: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Ready! Open http://%s.local in your browser\n", _host_ota);
      _disp.drawStr(0, 44, "http://espfcl-webupdate.local");
      _disp.drawStr(0, 55, WiFi.localIP().toString().c_str());
      _disp.sendBuffer();
  } else {
    Serial.println("WiFi Failed");
    _disp.drawStr(0, 22, "WiFi Failed");
    _disp.sendBuffer();
  }

while(true){
  server.handleClient();
  #ifdef ESP8266
   MDNS.update();
  #endif

}
}





//-------------------------------------
//----------------err------------------
//-------------------------------------
void FlexibleClockLibrary::getErr(const char* errMsg) {
    clearDisp();
    _disp.clearBuffer(); // clear the internal memory
    _disp.setFont(u8g2_font_6x10_tf); // choose a suitable font
    _disp.drawStr(0, 10, "Err! please reboot."); // write something to the internal memory
    Serial.println("Err! please reboot.");
    _disp.drawStr(0, 20, errMsg); // display error message
    Serial.println(errMsg);
    _disp.sendBuffer(); // transfer internal memory to the display
    delay(5000);
}
//-------------------------------------
//------------clear-disp---------------
//-------------------------------------
void FlexibleClockLibrary::clearDisp() {
    _disp.clearBuffer(); // Очищаємо буфер дисплея
    _disp.sendBuffer();  // Виводимо змінений (порожній) буфер на екран
    return;
}

//-------------------------------------
//----------clock-disp-----------------
//-------------------------------------

void FlexibleClockLibrary::ClockDisp(int ClockDispX, int ClockDispY, const uint8_t* backgroundBitmap, int bitmapWidth, int bitmapHeight){
    unsigned long lastActivityTime = millis(); // Час останньої активності
    bool displayOn = true; // Стан дисплея
while(true){

 _disp.clearBuffer(); 
 _disp.setFont(u8g2_font_t0_22b_tf); // Вибір шрифту
    while(true){
      delay(20);
 char timeBuffer[6];  // Буфер для збереження відформатованого часу
//    Serial.println(taskbar_show);
      // Малюємо фон, якщо передано бітмап
    if (backgroundBitmap != nullptr) {
      _disp.drawXBMP(0, 0, bitmapWidth, bitmapHeight, backgroundBitmap);
    }

   
       // _disp.clearBuffer();            // Очищення буфера
        sprintf(timeBuffer, "%02d:%02d", currentHours, currentMinutes); // Форматуємо час у вигляді "HH:MM"
        _disp.drawStr(ClockDispX, ClockDispY, timeBuffer);  // Виводимо час x:25, y:30

   if (digitalRead(_OKpin) == _OKsig) {
      delay(1000); 
      if (digitalRead(_OKpin) == _OKsig) { 
      delay(1000); return; }}

       _disp.sendBuffer(); 
       delay(100);
    
    
    if (millis() - lastActivityTime > 6000 && displayOn) {
        
        displayOn = false;
        while(displayOn == false){
            clearDisp();
            if (digitalRead(_OKpin) == _OKsig) {
                delay(200); 
                if (digitalRead(_OKpin) == _OKsig) { 
                    if (/*WiFi.status() != WL_CONNECTED &&*/ ClockUpdateMillis == 1) {
                        ClockUpdate(); 
                    }
                    delay(1000);
                     lastActivityTime = millis();
                     displayOn = true;
                      /*return;*/  break;  }}
            delay(100);
        }
    }

    


}

}}

    int UTC = 0; //your time zone (utc)

void FlexibleClockLibrary::ClockUpdate(){

  //--Clock
  cur_milllis = millis();
  if(cur_milllis - last_tick >= 1000){
    lsec = lsec + 1;
    if(lsec >= 60){
      lmin = lmin +1;
      lsec = 0;
    }
    if(lmin >= 60){
      lhour = lhour +1;
      lmin = 0;
    }
    if(lhour >= 24){
      lhour = 0;
    }
    last_tick = cur_milllis;
  }

  currentHours = lhour;
  currentMinutes = lmin;
}




//-------------------------------------
//--------drawLines--------------------
//-------------------------------------


void FlexibleClockLibrary::drawLines(const char* lineText) {
    //AI code
   // clearDisp(); // Очищаємо екран перед виведенням

    _disp.setFont(u8g2_font_6x10_tf); // Встановлюємо шрифт
    int x = 10; // Початкова координата X
    int y = 10; // Початкова координата Y
    int lineHeight = 12; // Висота рядка

    // Виведення тексту по рядках
    String line;
    for (int i = 0; lineText[i] != '\0'; i++) { // Use lineText instead of undefined 'text'
        if (lineText[i] == '\n') { // Якщо зустріли символ нового рядка
            _disp.drawStr(x, y, line.c_str()); // Виводимо рядок
            y += lineHeight; // Переходимо до наступного рядка
            line = ""; // Очищаємо рядок
        } else {
            line += lineText[i]; // Додаємо символ до рядка
        }
    }

    // Виводимо останній рядок, якщо він є
    if (line.length() > 0) {
        _disp.drawStr(x, y, line.c_str());
    }

    _disp.sendBuffer(); // Оновлюємо екран
    //AI code end
   
}








// taskbar-----------------------------
void FlexibleClockLibrary::taskbar_begin() {
  // taskbar_draw();
}




//-------------------------------------┌┬┐┌─┐┌─┐┬┌─┌┐ ┌─┐┬─┐  ┌┬┐┬─┐┌─┐┬ ┬
//----------taskbar_draw--------------- │ ├─┤└─┐├┴┐├┴┐├─┤├┬┘───││├┬┘├─┤│││
//------------------------------------- ┴ ┴ ┴└─┘┴ ┴└─┘┴ ┴┴└─  ─┴┘┴└─┴ ┴└┴┘
 int currentHours;
 int currentMinutes;
void FlexibleClockLibrary::taskbar_draw(int taskbar_y) {
    char timeBuffer[6];  // Буфер для збереження відформатованого часу
//    Serial.println(taskbar_show);
_disp.setFont(u8g2_font_6x10_tf); // Вибір шрифту
    
    if (taskbar_show == 1) {
       // _disp.clearBuffer();            // Очищення буфера
       // _disp.setFont(u8g2_font_6x10_tf); // Вибір шрифту
        sprintf(timeBuffer, "%02d:%02d", currentHours, currentMinutes); // Форматуємо час у вигляді "HH:MM"
        // Виводимо на дисплей
        _disp.drawStr(0, taskbar_y, timeBuffer);  // Виводимо час
        if(errType == 0) { _disp.drawXBMP(30, taskbar_y - 8, 8, 8, FlexibleClockLibrary::_err_bitmap_noerr); 
        }else if(errType == 1) { _disp.drawXBMP(30, taskbar_y - 8, 8, 8, FlexibleClockLibrary::_err_bitmap_err); 
        }else if(errType == 2) { _disp.drawXBMP(30, taskbar_y - 8, 8, 8, FlexibleClockLibrary::_err_bitmap_qestoin); 
        }else if(errType == 3) { _disp.drawXBMP(30, taskbar_y - 8, 8, 8, FlexibleClockLibrary::_err_bitmap_loading); 
        }else{_disp.drawXBMP(30, taskbar_y - 8, 8, 8, FlexibleClockLibrary::def_bitmap_def_unknown);}
        //wifi
        if(wifiType == 0) { _disp.drawXBMP(70, taskbar_y - 8, 8, 8, FlexibleClockLibrary::wifi_bitmap_nothing); 
        }else if(wifiType == 1) { _disp.drawXBMP(70, taskbar_y - 8, 8, 8, FlexibleClockLibrary::wifi_bitmap_wifi); 
        }else if(wifiType == 2) { _disp.drawXBMP(70, taskbar_y - 8, 8, 8, FlexibleClockLibrary::wifi_bitmap_ap); 
        }else if(wifiType == 3 && WiFi.status() != WL_CONNECTED) { _disp.drawXBMP(70, taskbar_y - 8, 8, 8, FlexibleClockLibrary::wifi_bitmap_disabled); 
        }else{_disp.drawXBMP(70, taskbar_y - 8, 8, 8, FlexibleClockLibrary::def_bitmap_def_unknown);}
        //bluetooth
        if(bluetoothType == 0) { _disp.drawXBMP(80, taskbar_y - 8, 8, 8, FlexibleClockLibrary::wifi_bitmap_nothing); 
        }else if(bluetoothType == 1) { _disp.drawXBMP(80, taskbar_y - 8, 8, 8, FlexibleClockLibrary::bluetooth_bitmap_bluetooth); 
        }else{_disp.drawXBMP(80, taskbar_y - 8, 8, 8, FlexibleClockLibrary::def_bitmap_def_unknown);}
        //remote
        if(remoteType == 0) { _disp.drawXBMP(90, taskbar_y - 8, 8, 8, FlexibleClockLibrary::wifi_bitmap_nothing); 
        }else if(remoteType == 1) { _disp.drawXBMP(90, taskbar_y - 8, 8, 8, FlexibleClockLibrary::remote_bitmap_remote); 
        }else{_disp.drawXBMP(90, taskbar_y - 8, 8, 8, FlexibleClockLibrary::def_bitmap_def_unknown);}
         
        _disp.sendBuffer();     // Відправляємо буфер на дисплей
    }
}


void FlexibleClockLibrary::drawBitmape(const unsigned char* bitmape) {
    _disp.drawXBMP(0, 0, 8, 8, bitmape);  // Малюємо зображення
    _disp.sendBuffer();    // Відправляємо буфер на дисплей
}


//-------------------------------------
//---------------wifi------------------
//-------------------------------------
void FlexibleClockLibrary::wifi_connect(const char* WIFI_SSID1, const char* WIFI_PASS1) {
    int attemptCounter = 0;
    
    WiFi.begin(WIFI_SSID1, WIFI_PASS1);
  while (WiFi.status() != WL_CONNECTED && attemptCounter < 30) { taskbar_draw(8);
    delay(500);
    Serial.print(".");
    attemptCounter++;
    errType = 3;
     if (digitalRead(_OKpin) == _OKsig) {
        errType = 2;
        taskbar_draw(8);
        //AI code
      delay(1000); // Затримка для запобігання повторенню натискань
      if (digitalRead(_OKpin) == _OKsig) { // Якщо кнопку ще тримають, вийти з циклу
       break; }}
       //end AI code
  }
  attemptCounter = 0;
  if(WiFi.status() == WL_CONNECTED){
   Serial.println("Connected");
  Serial.println(WiFi.localIP());
  errType = 0;
  wifiType = 1;
  taskbar_draw(8); 
  }else{Serial.println("no connected");}
  
  

}

void FlexibleClockLibrary::wifi_ap(const char* ssidAP, const char* passwordAP) {
    errType = 3;
    taskbar_draw(8); 
    delay(100);
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssidAP, passwordAP);

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  errType = 0;
  wifiType = 2;
  taskbar_draw(8); 
}

void FlexibleClockLibrary::wifi_scan() {
    // WiFi scan logic
}

void FlexibleClockLibrary::wifi_disable() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    wifiType = 0;
}

//-------------------------------------
//----------------ir-------------------
//-------------------------------------
void FlexibleClockLibrary::ir_rx() {
    // IR receive logic
}

void FlexibleClockLibrary::ir_tx() {
    // IR transmit logic
}

//-------------------------------------
//---------------MHz-------------------
//-------------------------------------
void FlexibleClockLibrary::mhz_tx() {
    // MHz transmit logic
}


//-------------------------------------
//---------------game-gonki------------
//-------------------------------------
void FlexibleClockLibrary::gamegonki(){clearDisp();
 int kamniY=8,kamniX=0,kamniPos=0,trigCarA=0,CarX=30,score=0,hscore=0,bthold=2000;
 unsigned long lastActivityTime = millis();
 for(int i=0;i=20000;i++){
    delay(200);
        
        if(digitalRead(_OKpin) == _OKsig) {
            if(trigCarA == 0){trigCarA=1;
              clearDisp(); 
             }else{trigCarA = 0;
                clearDisp();}}
        
 _disp.drawStr(0, 11, String(trigCarA).c_str());
 _disp.drawStr(0, 22, String(score).c_str());
 _disp.drawStr(0, 33, String(hscore).c_str());
 _disp.drawStr(CarX, 55,"A");
_disp.drawStr(kamniX, kamniY,"o");
     
if(trigCarA==0){CarX=40;}
if(trigCarA==1){CarX=70;}
  if(kamniY>=60){kamniPos=random(0,10); score+=1;
   if(kamniPos<5){kamniX=40;kamniY=0;}
   if(kamniPos>5){kamniX=70;kamniY=0;}
            clearDisp();}
kamniY+=8;
    
  if(kamniY>=55&&kamniX==CarX){clearDisp();
    if(score > hscore){hscore = score;}
     _disp.drawStr(0, 22, String(score).c_str());
     _disp.drawStr(0, 33, String(hscore).c_str());
     _disp.drawStr(10, 55,"game over");
     _disp.drawStr(20, 22,"score");
     _disp.drawStr(20, 33,"high-score");
     _disp.sendBuffer();
      delay(5000);
      score=0;
      if(digitalRead(_OKpin) == _OKsig){return;}
      clearDisp();}
 _disp.sendBuffer();
    
}}

//---------------
//--test-------
//---------------
void FlexibleClockLibrary::buzzertest(){
  delay(500);
  tone(_buzzerPin, 800, 200);
  delay(250);
  tone(_buzzerPin, 600, 200);
  delay(250);
  tone(_buzzerPin, 800, 200);
  delay(250);
  noTone(_buzzerPin);
  delay(250);
  tone(_buzzerPin, 400, 200);
  delay(250);
  noTone(_buzzerPin);
}
