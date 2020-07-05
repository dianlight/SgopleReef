#include "OTA.h"
#include "EvoDebug.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <functional>
#include "Config.h"
//#include "Display.h"




OTA::OTA() {
    // OTA
  debugD("OTA Subsystem start!");

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

  ArduinoOTA.setHostname(_SMT_HOST);
  

}

void OTA::onStart(){

  if (ArduinoOTA.getCommand() == U_FLASH) {
    type = "sketch";
  } else { // U_FS
    type = "filesystem";
  }
  processCallbacks(START,type);
  // NOTE: if updating FS this would be the place to unmount FS using FS.end()
  debugI("Start updating %s",type.c_str());
}

void OTA::onEnd(){
  processCallbacks(STOP);
  debugI("\nEnd");
}

void OTA::onProgress(unsigned int progress, unsigned int total){
  u8_t perc = progress / (total / 100);
  processCallbacks(PROGRESS,perc);
  if(progress % 10 == 0)debugI("%d/%d",progress,total);
}

void OTA::onError(ota_error_t error){
  processCallbacks(ERROR);
  debugE("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR) {
    debugE("Auth Failed");
    processCallbacks(ERROR,"Auth Failed");
  } else if (error == OTA_BEGIN_ERROR) {
    debugE("Begin Failed");
    processCallbacks(ERROR,"Begin Failed");
  } else if (error == OTA_CONNECT_ERROR) {
    debugE("Connect Failed");
    processCallbacks(ERROR,"Connect Failed");
  } else if (error == OTA_RECEIVE_ERROR) {
    debugE("Receive Failed");
    processCallbacks(ERROR,"End Failed");
  } else if (error == OTA_END_ERROR) {
    debugE("End Failed");
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
  MDNS.enableArduino(3232);  
  ArduinoOTA.begin();

//  checkOTATicker.attach_ms_scheduled(150,std::bind(&OTA::loopOTA,this));
  checkOTATicker.attach_ms(150, +[](OTA* instance) { instance->loopOTA(); }, this);

  _running = true;
  return _running;
}

bool OTA::stop(){
  checkOTATicker.detach();
  _running = false;
  return _running;
}

OTA ota = OTA();

