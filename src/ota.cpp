#include "ota.h"
//#include <ESP8266WiFi.h>
#include <ESPmDNS.h>
#include <functional>
//#include "Config.h"
//#include "Display.h"


OTA::OTA() {
  // OTA
  Serial.print("OTA Subsystem start!");

  // Port defaults to 8266
  ArduinoOTA.setPort(3232);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart(std::bind(&OTA::onStart, this));
  ArduinoOTA.onEnd(std::bind(&OTA::onEnd, this));
  ArduinoOTA.onProgress(std::bind(&OTA::onProgress,this,std::placeholders::_1,std::placeholders::_2));
  ArduinoOTA.onError(std::bind(&OTA::onError, this, std::placeholders::_1));

  ArduinoOTA.setHostname("SgopleReef");
  
}

void OTA::onStart(){
  processCallbacks(START);

  if (ArduinoOTA.getCommand() == U_FLASH) {
    type = "sketch";
  } else { // U_FS
    type = "filesystem";
  }

  Serial.printf("Start updating %s",type.c_str());
}

void OTA::onEnd(){
  processCallbacks(STOP);
  Serial.printf("\nEnd");
}

void OTA::onProgress(unsigned int progress, unsigned int total){
  Serial.printf("%d/%d",progress,total);
  processCallbacks(PROGRESS,progress,total);
}

void OTA::onError(ota_error_t error){
  
  Serial.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR) {
    Serial.printf("Auth Failed");
    processCallbacks(ERROR,"Auth Failed");
  } else if (error == OTA_BEGIN_ERROR) {
    Serial.printf("Begin Failed");
    processCallbacks(ERROR,"Begin Failed");
  } else if (error == OTA_CONNECT_ERROR) {
    Serial.printf("Connect Failed");
    processCallbacks(ERROR,"Connect Failed");
  } else if (error == OTA_RECEIVE_ERROR) {
    Serial.printf("Receive Failed");
    processCallbacks(ERROR,"Receive Failed");
  } else if (error == OTA_END_ERROR) {
    Serial.printf("End Failed");
    processCallbacks(ERROR,"End Failed");
  } else {
    processCallbacks(ERROR,"Unknown");  
  }
}


void OTA::loopOTA() {
  if (WiFi.status() == WL_CONNECTED){
    ArduinoOTA.handle();
  }
}

void OTA::processCallbacks(OTA_EVENT event,...){
  std::list<OtpEvent> :: iterator it; 
    for(it = callbacks.begin(); it != callbacks.end(); ++it) {
        va_list args;
        va_start(args,event);
        (*it)(event,args);
        va_end(args);
    }
}

bool OTA::start(){
  MDNS.end();
  MDNS.begin(ArduinoOTA.getHostname().c_str());
  MDNS.enableArduino(3232); // 8266  
  ArduinoOTA.begin();
//  checkOTATicker.attach_ms(150,+[](){ 
//    if (WiFi.status() == WL_CONNECTED){
//        ArduinoOTA.handle();
//    }
//  });
  _running = true;
  return _running;
}

bool OTA::stop(){
  checkOTATicker.detach();
  _running = false;
  return _running;
}

OTA ota = OTA();

