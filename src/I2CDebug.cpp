#include <Arduino.h>
#include <Wire.h>
#include "I2CDebug.h"
#include "EvoDebug.h"

void scanI2C(){
  int nDevices = 0;

  debugI("I2CScan Scanning...");
  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      debugI("I2C device found at address 0x%04x",address);
      ++nDevices;
    } else if (error == 4) {
      debugW("Unknown error at address 0x%04x",address);
    }
  }
  if (nDevices == 0) {
    debugI("I2CScan No I2C devices found");
  } else {
    debugI("done");
  }
  
}