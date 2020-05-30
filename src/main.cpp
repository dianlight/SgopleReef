#include <Arduino.h>
#include <WiFi.h>
#include "pinconfig.h"
#include "ota.h"

/**
 * TODO: 
 *  - Tarantura PT100 con 2 punti. 0 e Ambiente.
 **/

//needed for library
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager

AsyncWebServer server(80);
DNSServer dns;


// Display
#include <U8g2lib.h>
#include <Wire.h>
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA); // pin remapping with ESP8266 HW I2C
// U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0,  /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // pin remapping with ESP8266 HW I2C

// Fluxometer
volatile byte pulseCount;
float calibrationFactor = 7055; // Hz Spec https://italian.alibaba.com/product-detail/sen-hz41wa-electronic-water-flowmeter-magnetic-flow-sensor-water-flow-sensor-60583976123.html
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

// PT100
#include <driver/adc.h>
#include "esp_adc_cal.h"
// Gain = VREF/2^12 (Il sensore ADC ESP32 è ha 12bit)
#define DEFAULT_VREF 3300.0 //TL431
#define NO_OF_SAMPLES 128   //Multisampling
#define GAIN DEFAULT_VREF / 4095.0


void setup()
{
  Serial.begin(115200);

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info){
    Serial.println("Ottenuto IP!");
    ota.addOtaCallback([](OTA::OTA_EVENT event,va_list args){
      switch (event)
      {
      case OTA::OTA_EVENT::START/* constant-expression */:
        u8g2.clearDisplay();
        break;
      case OTA::OTA_EVENT::STOP/* constant-expression */:
        break;
      case OTA::OTA_EVENT::ERROR/* constant-expression */:
        u8g2.clearDisplay();
        u8g2.setCursor(0, 8);
        u8g2.printf("Error: %s",va_arg(args,char*));
        u8g2.sendBuffer();
        break;
      case OTA::OTA_EVENT::PROGRESS/* constant-expression */:
        u8g2.drawStr(0,8,"OTA Update..");
        u8g2.drawFrame(0,16,128,16);
        u8g2.drawBox(2,16+2,va_arg(args,int)/va_arg(args,int)*126,14);
        u8g2.sendBuffer();        
        break;
      }
    });
    ota.start();
  },WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect("SgopleReefAP");
  
  

//  MDNS.begin("SgopleReef");
//  MDNS.addService("http","tcp",80);

  // while (!Serial);
  Wire.begin(SDA, SCL);
  delay(1000);
  // I2C Scan
  Serial.println("\nI2C Scanner");
  byte error, address;
  int nDevices;
  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  // Display
  u8g2.begin();

  // Buttons
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  // Fluxometer
  pinMode(FLUX, INPUT);
  pulseCount = 0;
  attachInterrupt(digitalPinToInterrupt(FLUX), pulseCounter, CHANGE); // FALLING

  // Pump
  pinMode(PUMP, OUTPUT);

// Level
  pinMode(LEVEL, INPUT_PULLDOWN);

  // Peltier
  pinMode(PELTIER, OUTPUT);
}

void loop()
{
  ota.loopOTA();

  // PT100
  static byte sample = 0;
  static float temp = analogRead(THERM1) * GAIN;

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
                digitalRead(BUTTON1) == LOW ? "1" : "0",
                digitalRead(BUTTON2) == LOW ? "1" : "0",
                digitalRead(BUTTON3) == LOW ? "1" : "0");

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
    if (digitalRead(BUTTON3) == LOW)
    {
      digitalWrite(PUMP, HIGH);
    }
    else
    {
      digitalWrite(PUMP, LOW);
    }

    // LEVEL
    u8g2.setCursor(0, 8 * 3);
    u8g2.printf("Level:%s ", digitalRead(LEVEL) == LOW ? "LOW" : "OK");

    // PT100
    static float lastTemp = 0, delta = 0;
    if (sample > NO_OF_SAMPLES)
    {
      delta = abs(lastTemp - temp);
      sample = 0;
      lastTemp = temp;
      temp = analogRead(THERM1) * GAIN;
    }
    u8g2.setCursor(0, 8 * 4);
    u8g2.enableUTF8Print();
    u8g2.printf("Temp:%.1f °C D:%.2f °C", temp, delta);

    // PELTIER
    if (digitalRead(BUTTON2) == LOW)
    {
      digitalWrite(PELTIER, HIGH);
    }
    else
    {
      digitalWrite(PELTIER, LOW);
    }

    // Serial.println();
  }

  // PT100
  uint16_t newread = analogRead(THERM1) * GAIN;
  sample++;
  temp = (temp + newread) / 2;

  // Display
  u8g2.sendBuffer(); // transfer internal memory to the display

  delay(10);
}