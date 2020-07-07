#include "EvoDebug.h"
//#include <ESP8266mDNS.h>
#include <Wire.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>

#include "Config.h"
#include "EvoWifi.h"
#include "Display.h"
#include "OTA.h"
#include "I2CDebug.h"
#include "EvoWebserver.h"
#include "WaterLevel.h"
#include "HID.h"


/**
 * TODO: 
 *  - Tarantura PT100 con 2 punti. 0 e Ambiente.
 **/

//needed for library
/*
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager

AsyncWebServer server(80);
DNSServer dns;
*/

// Display
//#include <U8g2lib.h>
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA); // pin remapping with ESP8266 HW I2C
//// U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0,  /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // pin remapping with ESP8266 HW I2C




void setup()
{
  myConfig.init();
 /*
  Wire.begin(SDA, SCL);

  #ifdef DEBUG_I2C_SCAN
    delay(500);
    scanI2C();
  #endif
*/
  display.start();
  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    debugI("Wifi GotIP %s %s",WiFi.getHostname(),WiFi.localIP().toString().c_str());
    ota.addOtaCallback([](OTA::OTA_EVENT oevent,va_list args){
      switch (oevent)
      {
      case OTA::OTA_EVENT::START/* constant-expression */:
        evoWebserver.stop();
        waterLevel.stop();
        display.stop();
        break;
      case OTA::OTA_EVENT::ERROR/* constant-expression */:
        display.displayError(va_arg(args,char*));
        evoWebserver.start();
        waterLevel.start();
        display.start();
        break;
      case OTA::OTA_EVENT::STOP/* constant-expression */:
        display.displayError("Reboot..");
        delay(1000);
        break;
      case OTA::OTA_EVENT::PROGRESS/* constant-expression */:
          display.displayProgress(va_arg(args,int),va_arg(args,char*));
//        u8g2.drawStr(0,8,"OTA Update..");
//        u8g2.drawFrame(0,16,128,16);
//        u8g2.drawBox(2,16+2,va_arg(args,int)/va_arg(args,int)*126,14);
//        u8g2.sendBuffer();        
      break;
      }
    });
    ota.start();
    evoWebserver.start();
  },WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    evoWebserver.start();
  },WiFiEvent_t::SYSTEM_EVENT_AP_START);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    debugI("Wifi Event Disconnect %d",event);
  //  ota.stop();
  //  evoWebserver.stop();
  },WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

  waterLevel.start();



  // Peltier
  pinMode(PELTIER_PIN, OUTPUT);

  // Boot function
  if(hid.getCombo13()){
    debugD("AP config portal requested!");
    evoWifi.doAPConnect();
  } else if(!evoWifi.doSTAConnect()){
      debugW("Unable to connet to WiFi. Start in AP mode");
      evoWifi.doAPConnect();
  }

}

void loop()
{
  if (hid.getCombo12())
  { 
    debugI("Reboot");
    display.displayError("Reboot..");
    delay(1000);
    ESP.restart();
    while(true)delay(1000);
  } 

  /*
  // PT100
  static byte sample = 0;
  static float temp = analogRead(THERM1_PIN) * GAIN;

  static unsigned long last1s = 0;
  if (millis() - last1s > 1000)
  {
    last1s = millis();
    // Display
    u8g2.clearBuffer();             // clear the internal memory
                                    //   u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font/
    u8g2.setFont(u8g2_font_5x8_tf); // choose a suitable font/
                                    //   u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
                                    //   u8g2.sendBuffer();					// transfer internal memory to the display

    // Buttons
    u8g2.setCursor(0, 8);
    u8g2.printf("Buttons: %s %s %s",
                digitalRead(BUTTON1_PIN) == LOW ? "1" : "0",
                digitalRead(BUTTON2_PIN) == LOW ? "1" : "0",
                digitalRead(BUTTON3_PIN) == LOW ? "1" : "0");

    // Flux
    static unsigned long currentMillis, previousMillis = 0, totalMilliLitres = 0;
    currentMillis = millis();
    if (currentMillis - previousMillis > 1000)
    {
      byte pulse1Sec = pulseCount;
      pulseCount = 0;
      totalMilliLitres += (1000.0 / calibrationFactor) * pulse1Sec;
      float flowRate = (1000.0 / calibrationFactor) * (pulse1Sec / ((currentMillis - previousMillis) / 1000.0));
      previousMillis = millis();
      u8g2.setCursor(0, 8 * 2);
      u8g2.printf("Flux:%.2f ml/s %ld ml ", flowRate, totalMilliLitres);
    }

    // PUMP
    if (digitalRead(BUTTON3_PIN) == LOW)
    {
      digitalWrite(PUMP_PIN, HIGH);
    }
    else
    {
      digitalWrite(PUMP_PIN, LOW);
    }

    // LEVEL
    u8g2.setCursor(0, 8 * 3);
    u8g2.printf("Level:%s ", digitalRead(LEVEL_PIN) == LOW ? "LOW" : "OK");

    // PT100
    static float lastTemp = 0, delta = 0;
    if (sample > NO_OF_SAMPLES)
    {
      delta = abs(lastTemp - temp);
      sample = 0;
      lastTemp = temp;
      temp = analogRead(THERM1_PIN) * GAIN;
    }
    u8g2.setCursor(0, 8 * 4);
    u8g2.enableUTF8Print();
    u8g2.printf("Temp:%.1f °C D:%.2f °C", temp, delta);

    // PELTIER
    if (digitalRead(BUTTON2_PIN) == LOW)
    {
      digitalWrite(PELTIER_PIN, HIGH);
    }
    else
    {
      digitalWrite(PELTIER_PIN, LOW);
    }

    // Serial.println();
  }

  // PT100
  uint16_t newread = analogRead(THERM1_PIN) * GAIN;
  sample++;
  temp = (temp + newread) / 2;

  // Display
  u8g2.sendBuffer(); // transfer internal memory to the display

  delay(10);
  */
}