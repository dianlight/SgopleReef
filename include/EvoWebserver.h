#pragma once
#include "EvoDebug.h"
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include "EvoStartableInterface.h"


class EvoWebserver: public EvoStopable {
    public:
        EvoWebserver();

        bool start();
        bool stop();

    private:

        Ticker webServerTicker;
        AsyncWebServer server{80};
        #ifdef EVODEBUG_REMOTE
            AsyncWebSocket wsLog{"/log"};
        #endif
        AsyncWebSocket wsScan{"/scan"};
        //AsyncEventSource events{"/events"};

        void loopWebServer();

        void handleOstatData(AsyncWebServerRequest *request);
        void handleLoadWifiData(AsyncWebServerRequest *request);
        void handleScanWifiData(AsyncWebServerRequest *request);
        void handleSaveWifiData(AsyncWebServerRequest *request);
//        void handleWPSData(AsyncWebServerRequest *request);
//        void handleDisplayData(AsyncWebServerRequest *request);
        void handleDisplayBmpData(AsyncWebServerRequest *request);
        void handleLoadData(AsyncWebServerRequest *request);
//        void handleLoadProgramData(AsyncWebServerRequest *request);
        void handleSaveData(AsyncWebServerRequest *request);
//        void handleSaveProgramData(AsyncWebServerRequest *request);
        bool handleFileRead(AsyncWebServerRequest *request);
        void handleCmdData(AsyncWebServerRequest *request);
        void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

        void sendScanResult(int networksFound);

};

 #ifdef EVODEBUG_REMOTE
         class WebsocketEvoAppender: public EvoAppender {
            public:
                WebsocketEvoAppender(AsyncWebSocket &ws): ws(ws){}

                void begin(){
                    ws.printfAll("///// WebsocketEvoAppender start /////");
                }

                void end(){
                    ws.printfAll("///// WebsocketEvoAppender stop  /////");
                }

                size_t write(uint8_t c){
                    ws.textAll(&c,1);
                    return 1;
                }

                size_t write(const uint8_t *buffer, size_t size){
                    ws.textAll((char *)buffer,size);
                    return size;
                }
            
            private:
                AsyncWebSocket &ws;
        };
 #endif

 extern EvoWebserver evoWebserver;