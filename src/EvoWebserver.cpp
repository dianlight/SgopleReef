#include <EvoWebserver.h>
#include "EvoDebug.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <strings.h>
#include <U8g2lib.h>
#include <AsyncJson.h>
#include "Config.h"
#include "Display.h"
#include "WaterLevel.h"


#include "web_static/web_server_static_files.h"

//extern const uint8_t aws_root_ca_pem_start[] asm("_binary_data_src_aws_root_ca_pem_start");

void EvoWebserver::handleDisplayBmpData(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("image/x‑portable‑bitmap");
  response->addHeader("Content-Disposition","attachment; filename=\"screen.pbm\"");
  display.screeshotbmp(*response);
  request->send(response);
}

void EvoWebserver::handleLoadData(AsyncWebServerRequest *request){

    StaticJsonDocument<1024> root;
    /*
    root["teco"] = myConfig.get()->targetTemp[0];
    root["tnorm"] = myConfig.get()->targetTemp[1];
    root["tconf"] = myConfig.get()->targetTemp[2];
    root["aautotimeout"] = myConfig.get()->returnAutoTimeout;
    */
    root["dsleep"] = myConfig.get()->displaySleep;
    root["doff"] = myConfig.get()->displayPowerOff;
    /*
    root["tadelta"] = myConfig.get()->awayModify;
    root["tamode"] = myConfig.get()->awayMode;
    root["tprec"] = myConfig.get()->tempPrecision;
    root["sres"] = myConfig.get()->minSwitchTime;
    root["nname"] = myConfig.get()->mqtt_client_id;
    root["saddress"] = myConfig.get()->mqtt_server;
    root["port"] = myConfig.get()->mqtt_port;
    root["user"] = myConfig.get()->mqtt_user;
    root["password"] = myConfig.get()->mqtt_password;
    root["tprefix"] = myConfig.get()->mqtt_topic_prefix;
    */
    root["tzero"] = myConfig.get()->tZeroVolt;
    root["tonet"] = myConfig.get()->tOneTemp;
    root["tone"] = myConfig.get()->tOneVolt;

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(root,*response);
    request->send(response);
}

/*
void EvoWebserver::handleLoadProgramData(AsyncWebServerRequest *request){
  AsyncJsonResponse *response = new AsyncJsonResponse(true,2048U);
  JsonArray program = response->getRoot();
  AsyncWebParameter* day = request->getParam("day");
  for(u8_t h=0; h < (24*4); h++){
    program.add(myConfig.get()->weekProgram[day->value().toInt()].hourQuarterHolds[h]);
  }
  response->setLength();
  request->send(response);
}
*/

void EvoWebserver::handleSaveData(AsyncWebServerRequest *request){
        debugI("Ricevuta chiamata di Save!");
        debugI("Ricevuti %d parametri",request->args());
        for(size_t i=0; i< request->args(); i++){
            debugI("%s=%s",request->argName(i).c_str(), request->arg(i).c_str());
        }
        /*
        myConfig.get()->targetTemp[0] = request->arg("teco").toFloat();
        myConfig.get()->targetTemp[1] = request->arg("tnorm").toFloat();
        myConfig.get()->targetTemp[2] = request->arg("tconf").toFloat();
        myConfig.get()->returnAutoTimeout = request->arg("aautotimeout").toInt();
        */
        myConfig.get()->displaySleep = request->arg("dsleep").toInt();
        myConfig.get()->displayPowerOff = request->arg("doff").toInt();
        /*
        myConfig.get()->awayModify = request->arg("tadelta").toFloat();
        myConfig.get()->awayMode = request->arg("tamode").toFloat();
        myConfig.get()->tempPrecision = request->arg("tprec").toFloat();
        myConfig.get()->minSwitchTime = request->arg("sres").toFloat();
        strcpy(myConfig.get()->mqtt_client_id,request->arg("nname").c_str());
        strcpy(myConfig.get()->mqtt_server,request->arg("saddress").c_str());
        strcpy(myConfig.get()->mqtt_port,request->arg("port").c_str());
        strcpy(myConfig.get()->mqtt_user,request->arg("user").c_str());
        strcpy(myConfig.get()->mqtt_password,request->arg("password").c_str());
        strcpy(myConfig.get()->mqtt_topic_prefix,request->arg("tprefix").c_str());
        */
         myConfig.get()->tZeroVolt = request->arg("tzero").toInt();
         myConfig.get()->tOneTemp = request->arg("tonet").toFloat();
         myConfig.get()->tOneVolt = request->arg("tone").toInt();
 

        myConfig.saveConfig();

        request->send(204);

}

/*
void EvoWebserver::handleSaveProgramData(AsyncWebServerRequest *request){
        debugI("Ricevuta chiamata di Save Program!");
        debugI("Ricevuti %d parametri",request->args());
        for(size_t i=0; i< request->args(); i++){
            if(request->argName(i).startsWith("plain"))continue;
            debugI("%s=%s",request->argName(i).c_str(), request->arg(i).c_str());
            u8_t day = atoi(&request->argName(i).c_str()[4]);
            debugI("Working on day %d '%s'",day,request->arg(i).c_str());
            StaticJsonDocument<1536> doc;
            DeserializationError err = deserializeJson(doc, request->arg(i).c_str());
            if (err) {
              debugI("deserializeJson() failed with code: %s ",err.c_str());
              request->send(500,err.c_str());
            } else {
              for(u8_t hq=0; hq < 24*4; hq++){
                debugI("day:%d hq:%d = %d",day,hq,doc[hq].as<u8_t>());
   //             myConfig.get()->weekProgram[day].hourQuarterHolds[hq]=doc[hq].as<u8_t>();
              }
            }
        }
        myConfig.saveConfig();

        request->send(204);
}
*/

void EvoWebserver::handleOstatData(AsyncWebServerRequest *request){
    StaticJsonDocument<100> root;
    if(WiFi.getMode() != WIFI_STA){
      root["otab"] = "wificonfig";
    } else {
      #ifdef EVODEBUG_REMOTE
        root["otab"] = "debug";
      #else 
        root["otab"] = "home";
      #endif
//      root["hold"] = CURRENT_HOLD_STR;
//      root["away"] = thermostat.getAway();
      root["mode"] = CURRENT_MODE_STR;
    }

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(root,*response);
    request->send(response);
}

void EvoWebserver::handleLoadWifiData(AsyncWebServerRequest *request){
    StaticJsonDocument<87> root;
    root["ssid"] = WiFi.SSID();
    root["wkey"] = "*Secret*";

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    serializeJson(root,*response);
    request->send(response);
}

/*
void EvoWebserver::sendScanResult(int networksFound){
  Serial.printf("%d network(s) found\n", networksFound);
  for (int i = 0; i < networksFound; i++)
  {
    debugD("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    StaticJsonDocument<115> root;
    root["i"] = i;
    root["ssid"] = WiFi.SSID(i);
    root["dBm"] = WiFi.RSSI(i); // Less is better!
    root["open"] = (WiFi.encryptionType(i) == wifi_auth_mode_t::WIFI_AUTH_OPEN);
    char buffer[124];
    serializeJson(root,buffer);
    debugD("Send WiFi Info: %s",buffer);
    wsScan.printfAll(buffer);
  }
  wsScan.printfAll("{\"end\":true}");
}
*/


void EvoWebserver::handleScanWifiData(AsyncWebServerRequest *request){
  debugD("Ricevuta richiesta di ScanWifi");
  WiFi.scanNetworks(true,false,false);
  //std::bind(&EvoWebserver::sendScanResult,this,std::placeholders::_1),false);
  request->send(204);
}

void EvoWebserver::handleSaveWifiData(AsyncWebServerRequest *request){
  debugI("Ricevuta chiamata di WifiSave!");
  debugI("Ricevuti %d parametri",request->args());
  #ifdef DEBUG_WEBSERVER
    for(size_t i=0; i< request->args(); i++){
        debugD("%s=%s",request->argName(i).c_str(), request->arg(i).c_str());
    }
  #endif
  WiFi.persistent(true);
  bool ok = WiFi.begin(request->arg("ssid").c_str(),request->arg("wkey").c_str());
  debugI("Connessone %s -> AP %s ",ok?"SUCCESS":"FAIL",request->arg("ssid").c_str());
  if(ok){
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }

  request->send(204);
}

/*
void EvoWebserver::handleWPSData(AsyncWebServerRequest *request){
  debugI("Ricevuta chiamata di WPS!");

  WiFi.beginWPSConfig();

  request->send(204);
}
*/

void EvoWebserver::handleCmdData(AsyncWebServerRequest *request){
  String value = request->arg("value");
  if(request->arg("cmd") == "mode"){
    for (byte i = 0; i < Config::MODES::M_SIZE; i++){
      if(value == String(myConfig.MODES_NAME[i])){
        waterLevel.setMode((Config::MODES)i);
        break;  
      }
    }
  /*
  }  else if(request->arg("cmd") == "hold"){
    String value = request->arg("value");
    for (byte i = 0; i < Config::HOLDS::H_SIZE; i++){
       if(value == String(myConfig.HOLDS_NAME[i])){
        thermostat.setHold((Config::HOLDS)i);
        break;  
      }
    }
  } else if(request->arg("cmd") == "away"){
    bool value = request->arg("value").equalsIgnoreCase("True");
    thermostat.setAway(value);
  */
  } else {
    debugE("Unknown command %s",request->arg("cmd").c_str());
    request->send(504);
  }
  request->send(204);

}



bool EvoWebserver::handleFileRead(AsyncWebServerRequest *request){  // send the right file to the client (if it exists)
  String path = request->url();
  debugD("handleFileRead: %s %s",path.c_str(),request->host().c_str());
  if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
  for(byte i=0; i < STATIC_FILES_SIZE; i++){
    if(staticFiles[i].filename == path || staticFiles[i].filename == path+".gz"){
      AsyncWebServerResponse *response = request->beginResponse_P(200, staticFiles[i].mimetype, staticFiles[i].data, staticFiles[i].length);
      if(staticFiles[i].filename.endsWith(".gz"))response->addHeader("Content-Encoding", "gzip");
      request->send(response);
      return true;
    } 
  }

  debugW("File Not Found: %s",path.c_str());
  if(WiFi.getMode() == wifi_mode_t::WIFI_MODE_APSTA){
    debugI("AP mode. Captive portal on. Redirect to config page...");
    AsyncWebServerResponse *response = request->beginResponse(302,"text/plain");
    response->addHeader("Location", String("http://") + WiFi.softAPIP().toString());
    request->send(response);
    return true;
  }
  return false;                                          // If the file doesn't exist, return false
}

void EvoWebserver::onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    debugD("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    debugD("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    debugD("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    debugD("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      debugD("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      debugD("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          debugD("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        debugD("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      debugD("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      debugD("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        debugD("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          debugD("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}


EvoWebserver::EvoWebserver(){

    MDNS.addService("http", "tcp", 80);   // Web server

    server.on("/ostat",HTTP_GET,std::bind(&EvoWebserver::handleOstatData,this,std::placeholders::_1));
    server.on("/cmd",HTTP_GET,std::bind(&EvoWebserver::handleCmdData,this,std::placeholders::_1));
    server.on("/load",HTTP_GET,std::bind(&EvoWebserver::handleLoadData,this,std::placeholders::_1));
    server.on("/save",HTTP_POST, std::bind(&EvoWebserver::handleSaveData,this,std::placeholders::_1));
  //  server.on("/loadP",HTTP_GET, std::bind(&EvoWebserver::handleLoadProgramData,this,std::placeholders::_1));
  //  server.on("/saveP",HTTP_POST, std::bind(&EvoWebserver::handleSaveProgramData,this,std::placeholders::_1));
    server.on("/loadW",HTTP_GET, std::bind(&EvoWebserver::handleLoadWifiData,this,std::placeholders::_1));
    server.on("/scanW",HTTP_GET, std::bind(&EvoWebserver::handleScanWifiData,this,std::placeholders::_1));
    server.on("/saveW",HTTP_POST, std::bind(&EvoWebserver::handleSaveWifiData,this,std::placeholders::_1));
  //  server.on("/wps",HTTP_GET, std::bind(&EvoWebserver::handleWPSData,this,std::placeholders::_1));
    server.on("/screenpbm",HTTP_GET, std::bind(&EvoWebserver::handleDisplayBmpData,this,std::placeholders::_1));
   
    server.onNotFound([this](AsyncWebServerRequest *request) {                              // If the client requests any URI
        if (!handleFileRead(request))                  // send it if it exists
          request->send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    wsScan.onEvent(std::bind(&EvoWebserver::onWsEvent,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5,std::placeholders::_6));
    server.addHandler(&wsScan);

    #ifdef EVODEBUG_REMOTE
      wsLog.onEvent(std::bind(&EvoWebserver::onWsEvent,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5,std::placeholders::_6));
      server.addHandler(&wsLog);
      EvoAppender* webserverAppender =new WebsocketEvoAppender(wsLog);
      evoDebug.addEvoAppender(webserverAppender);
    #endif

    WiFi.onEvent([&](WiFiEvent_t event, WiFiEventInfo_t info){
      int16_t networksFound = WiFi.scanComplete();
      debugD("%d network(s) found\n", networksFound);
      for (int i = 0; i < networksFound; i++)
      {
        debugD("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == wifi_auth_mode_t::WIFI_AUTH_OPEN ? "open" : "");
        StaticJsonDocument<115> root;
        root["i"] = i;
        root["ssid"] = WiFi.SSID(i);
        root["dBm"] = WiFi.RSSI(i); // Less is better!
        root["open"] = (WiFi.encryptionType(i) == wifi_auth_mode_t::WIFI_AUTH_OPEN);
        char buffer[124];
        serializeJson(root,buffer);
        debugD("Send WiFi Info: %s",buffer);
        wsScan.printfAll(buffer);
      }
      wsScan.printfAll("{\"end\":true}");      
    },SYSTEM_EVENT_SCAN_DONE);
}

bool EvoWebserver::stop(){
    webServerTicker.detach();
    server.end();
    _running = false;
    debugI("HTTP server stopped");
    return _running;
}

bool EvoWebserver::start(){
    server.begin();
//    webServerTicker.attach_ms(300,std::bind(&EvoWebserver::loopWebServer,this));
    webServerTicker.attach_ms(300, +[](EvoWebserver* instance) { instance->loopWebServer(); }, this);
    _running = true;
    debugI("HTTP server started");
    return _running;
}


void EvoWebserver::loopWebServer(){
  #ifdef EVODEBUG_REMOTE
  wsLog.cleanupClients();
  #endif
  wsScan.cleanupClients();
}

EvoWebserver evoWebserver = EvoWebserver();
