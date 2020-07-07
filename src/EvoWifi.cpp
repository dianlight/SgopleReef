#include "EvoWifi.h"
#include "EvoDebug.h"
#include <Arduino.h>
#include <WiFi.h>

#include <WiFiUdp.h>
#include "Config.h"
#include "Display.h"
#include "EvoWebserver.h"


EvoWifi::EvoWifi(){
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
}

bool EvoWifi::doSTAConnect(){
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin("Casa","&&MartyEli767");
    delay(500);
    int8_t status = WiFi.waitForConnectResult();
    if(status != WL_CONNECTED){
      switch (status)
      {
      case WL_IDLE_STATUS:
        debugW("WiFi connection error or not configured! IDLE");
        display.displayError("WiFi conncection error! IDLE");
        break;
      case WL_NO_SSID_AVAIL:
        debugW("WiFi connection error or not configured! NO_SSD");
        display.displayError("WiFi conncection error! NO_SSD");
        break;
      case WL_CONNECT_FAILED:
        debugW("WiFi connection error or not configured! FAIL");
        display.displayError("WiFi conncection error! FAIL");
        break;
      case WL_CONNECTION_LOST:
        debugW("WiFi connection error or not configured! LOST");
        display.displayError("WiFi conncection error! LOST");
        break;
      case WL_DISCONNECTED:
        debugW("WiFi connection error or not configured! DICONNECTED");
        display.displayError("WiFi conncection error! DICONNECTED");
        break;
      case WL_SCAN_COMPLETED:
      default:
        debugW("WiFi connection error or not configured! Status=0x%x", status);
        display.displayError("WiFi conncection error!");
        break;
      }
      return false;
    } 
    return true;
}

bool EvoWifi::doAPConnect(){
  WiFi.mode(WIFI_AP_STA);
  if(WiFi.softAP(_SMT_HOST)){
    debugI("Config AP:%s IP:%s",WiFi.SSID().c_str(),WiFi.softAPIP().toString().c_str());
    display.bootAPDisplay(WiFi.SSID().c_str());
    dnsServer.start(53, "*", WiFi.softAPIP());
    evoWebserver.start();
//    dnsTicker.attach_ms(200,std::bind(&EvoWifi::loopEvent,this));
    dnsTicker.attach_ms(200, +[](EvoWifi* instance) { instance->loopEvent(); }, this);

    return true;
  }
  debugE("Unable to start SoftAP!");
  return false;
}

void EvoWifi::loopEvent(){
  dnsServer.processNextRequest();
}

EvoWifi evoWifi = EvoWifi();
