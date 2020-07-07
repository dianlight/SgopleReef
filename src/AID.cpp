#include "AID.h"
#include "Constants.h"
#include "Config.h"

// PT100
#include <driver/adc.h>
#include "esp_adc_cal.h"
// Gain = VREF/2^12 (Il sensore ADC ESP32 è ha 12bit)
#define DEFAULT_VREF 3300.0 //TL431
#define NO_OF_SAMPLES 128   //Multisampling
//#define GAIN DEFAULT_VREF / 4095.0

#define POINT_0  970
#define POINT_28_5  1655
#define GAIN  24.00

/*
void IRAM_ATTR HID::interrupt_handler_static(void *arg)
{
    reinterpret_cast<HID *>(arg)->buttonPressed();
}

void IRAM_ATTR HID::buttonPressed()
{
    //    debugD("Pressed Button");
    lastAction = millis();
    status = 0xFF & (!digitalRead(BUTTON1_PIN) << BUTTON_SHIFT[0] |
                     !digitalRead(BUTTON2_PIN) << BUTTON_SHIFT[1] |
                     !digitalRead(BUTTON3_PIN) << BUTTON_SHIFT[2]);
}
*/
AID::AID()
{
    pinMode(THERM1_PIN, INPUT);
}

float AID::getTemp()
{
// PT100
static float lasttemp=0;
static int counter=100;

if (counter<100){
    counter++;
    return lasttemp;
} else {
    counter=0;
}
          uint16_t rtemp = analogRead(THERM1_PIN);
          lasttemp=(rtemp - POINT_0) / GAIN;
        //  debugD("Temp vref %d",rtemp);
          return lasttemp;

}

AID aid = AID();