#pragma once
#include <Ticker.h>

#include <DNSServer.h>

class EvoWifi {
    public:
        EvoWifi();

        bool doSTAConnect();
        bool doAPConnect();

    private:

        DNSServer dnsServer;
        Ticker dnsTicker;

        void loopEvent();
};

extern EvoWifi evoWifi;
