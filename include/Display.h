#pragma once
#include <Stream.h>
#include <U8g2lib.h>
#include <Ticker.h>
#include "EvoStartableInterface.h"


#define U8_Width 128
#define U8_Height 32

class Display: public EvoStopable {
    public:

        Display();
        bool start();
        bool stop();
        void clearDisplay();
        void bootAPDisplay(String AP);
        void bootConnectedDisplay();
        void displayError(String Error);  
        void displayProgress(uint8_t perc,String type);

        void screeshotbmp(Print &p);

    private:
        typedef byte u8g2_Bitmap[U8_Height][U8_Width];

        U8G2 _u8g2;
        
        Ticker sleepModeTicker,displayRefreshTicker;

//        int8_t lastEncPosition = 0;

        void sleepModeDisplay();

        void loopDisplay();
};

extern Display display;

