#include <Arduino.h>
#include "pinconfig.h"

// Display
#include <U8g2lib.h>
#include <Wire.h>
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

// Fluxometer
volatile byte pulseCount;
float calibrationFactor = 7055; // Hz Spec https://italian.alibaba.com/product-detail/sen-hz41wa-electronic-water-flowmeter-magnetic-flow-sensor-water-flow-sensor-60583976123.html
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

// PT100
#ifdef ESP32
  #include <driver/adc.h>
  #include "esp_adc_cal.h"
#endif
// Gain = VREF/2^12 (Il sensore ADC ESP32 è ha 12bit)
#define DEFAULT_VREF    1128.0      //Use spefuse.py --port /dev/cu.SLAB_USBtoUART adc_info 
#define NO_OF_SAMPLES   128         //Multisampling
#define GAIN DEFAULT_VREF/4096.0



void setup() {
  Serial.begin(9600);
 // while (!Serial);    
  Wire.begin(SDA,SCL);
  delay(1000);
  // I2C Scan
  Serial.println("\nI2C Scanner");
  byte error, address;
  int nDevices;
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
      Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
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
  #ifdef ESP32
    pinMode(LEVEL, INPUT_PULLDOWN);
  #elif ESP8266
    pinMode(LEVEL, INPUT_PULLDOWN_16);
  #endif

  // PT100
  #ifdef ESP32
    esp_err_t status = adc2_vref_to_gpio(VREF);
    if (status == ESP_OK) {
          Serial.printf("v_ref routed to GPIO %d\n",analogRead(VREF));
    } else {
          Serial.printf("failed to route v_ref\n");
    }
  #endif

  // Peltier
  pinMode(PELTIER,OUTPUT);


}

void loop() {

  // PT100
  static byte sample = 0;
  static float temp = analogRead(THERM3)*GAIN;
 

  static unsigned long last1s = 0;
  if(millis() - last1s > 1000){
    last1s = millis();
    // Display
    u8g2.clearBuffer();					// clear the internal memory
 //   u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font/
    u8g2.setFont(u8g2_font_5x8_tf);	// choose a suitable font/
 //   u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
 //   u8g2.sendBuffer();					// transfer internal memory to the display

    // Buttons
    u8g2.setCursor(0,8);
    u8g2.printf("Buttons: %s %s %s ",
      digitalRead(BUTTON1) == LOW?"1":"0",
      digitalRead(BUTTON2) == LOW?"1":"0",
      digitalRead(BUTTON3) == LOW?"1":"0"
    );  

    // Flux
    static unsigned long currentMillis,previousMillis =0,totalMilliLitres=0;
    currentMillis = millis();
    if (currentMillis - previousMillis > 1000) {
      byte pulse1Sec = pulseCount;
      pulseCount = 0;
      totalMilliLitres += (1000.0 / calibrationFactor )  * pulse1Sec;
      float flowRate = (1000.0 / calibrationFactor ) * (pulse1Sec / ((currentMillis - previousMillis)/1000.0)) ;
      previousMillis = millis();
      u8g2.setCursor(0,8*2);
      u8g2.printf("Flux:%.2f ml/s %ld ml ", flowRate,totalMilliLitres);
    }

    // PUMP
    if(digitalRead(BUTTON3) == LOW){
      digitalWrite(PUMP,LOW);
    } else {
      digitalWrite(PUMP,HIGH);
    }

    // LEVEL
    u8g2.setCursor(0,8*3);
    u8g2.printf("Level:%s ",digitalRead(LEVEL) == HIGH?"LOW":"OK");

    // PT100
    static float lastTemp = 0, delta = 0;
    if(sample > NO_OF_SAMPLES){
      delta = abs(lastTemp-temp);
      sample=0;
      lastTemp = temp;
      temp = analogRead(THERM3)*GAIN;
    }
    u8g2.setCursor(0,8*4);
    u8g2.enableUTF8Print();
    u8g2.printf("Temp:%.1f °C D:%.2f °C",temp,delta);

    // PELTIER
    if(digitalRead(BUTTON2) == LOW){
      digitalWrite(PELTIER,HIGH);
    } else {
      digitalWrite(PELTIER,LOW);
    }


   // Serial.println();
  }

  // PT100
  uint16_t newread = analogRead(THERM3);
  sample++;
  temp = (temp + (newread*GAIN)) / 2;
  
  // Display
  u8g2.sendBuffer();					// transfer internal memory to the display

  delay(10);    
}