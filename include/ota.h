#pragma once
#include <Ticker.h>
#include <list> 
#include <ArduinoOTA.h>

class EvoStartable {
    public:
        virtual ~EvoStartable() {}
        virtual bool start() = 0;
        bool isRunning(){ return _running;};
        bool _running = false;
};

class EvoStopable: public EvoStartable {
    public:
        virtual ~EvoStopable() {}
        virtual bool stop() = 0;
};

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
        void loopOTA();
    
    private:
        Ticker checkOTATicker;

        std::list<OtpEvent> callbacks;

        String type;

        void onStart();
        void onEnd();
        void onProgress(unsigned int progress, unsigned int total);
        void onError(ota_error_t error);

        void processCallbacks(OTA_EVENT event,...);

};

extern OTA ota;