#pragma once
#include <Arduino.h>
#include <Preferences.h>

#define STR2(x) #x
#define STR(x) STR2(x)
#ifndef SMT_VERSION
    #define _SMT_VERSION ""
#else
    #define _SMT_VERSION STR(SMT_VERSION)   
#endif

#ifndef SMT_HOST
    #define _SMT_HOST "SgopleReef"
#else
    #define _SMT_HOST  STR(SMT_HOST)
#endif

#ifdef EVODEBUG
    #define DEBUG_I2C_SCAN
//    #define DEBUG_I2C_IN
//    #define DEBUG_I2C_OUT
//    #define DEBUG_DISPLAY
//    #define DEBUG_MQTT
    #define DEBUG_EVENT
//    #define DEBUG_WEBSERVER
#endif

// PIN Configuration

// Buttons
#define BUTTON1_PIN 18
#define BUTTON2_PIN 19
#define BUTTON3_PIN 5

// Flussometro
#define FLUX_PIN 4

// Relay Pump
#define PUMP_PIN 15

// Level Sensor
#define LEVEL_PIN 27

//PT100
#define THERM1_PIN A6        // 34
#define THERM2_PIN A7        // 35
#define THERM3_PIN A4        // 33
#define VREF_PIN   GPIO_NUM_25 // Valid are 25/26/27

// Relay Peltier
#define PELTIER_PIN 12

// General Config
#define CONFIG_VERSION  "001"

// 
#define CURRENT_MODE_STR    myConfig.MODES_NAME[myConfig.get()->mode]


#include "Constants.h"


class Config {
    public:

        enum __attribute__((__packed__)) MODES {
            OFF,     
            MANUAL,  
            AUTO,

            M_SIZE    
        };

        const char* MODES_NAME[3] = {"Off","Manual","Auto"};

        struct __attribute__((packed, aligned(1))) StoreStruct {
            char version[4]                 = CONFIG_VERSION;
            size_t configSize               = sizeof(StoreStruct);
            byte mode                       = MODES::AUTO;

            byte displaySleep           = 35;
            byte displayPowerOff        = 60;
            byte displayContrast        = 200;

            uint16_t  tZeroVolt         = 0;
            float     tOneTemp          = 22.0;
            uint16_t  tOneVolt          = 3000;
        };

        Config();
        void init();
        void saveConfig();
        StoreStruct *get();

    private:
        Preferences prefs;
        bool _started = false;
};

extern Config myConfig;


