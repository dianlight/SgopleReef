#pragma once
#include <Ticker.h>
#include <list> 
#include <ArduinoOTA.h>
#include "EvoStartableInterface.h"



class OTA: public EvoStopable {
    public:
        enum __attribute__((__packed__)) OTA_EVENT {
            START,
            STOP,
            ERROR,
            PROGRESS
        };

        typedef void (*OtpEvent)(OTA_EVENT event,va_list args);

        OTA();
        void addOtaCallback(OtpEvent otaEventCallback){ callbacks.push_back(otaEventCallback); };

        bool start();
        bool stop();
    
    private:
        Ticker checkOTATicker;

        std::list<OtpEvent> callbacks;

        String type;

        void loopOTA();
        void onStart();
        void onEnd();
        void onProgress(unsigned int progress, unsigned int total);
        void onError(ota_error_t error);

        void processCallbacks(OTA_EVENT event,...);

};

extern OTA ota;