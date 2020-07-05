#pragma once
#include <string.h>
#include <Ticker.h>
#include "Config.h"
#include "EvoStartableInterface.h"

/**
 * @brief 
 * 
 */
/*
#define CURRENT_MODE_STR myConfig.MODES_NAME[myConfig.get()->mode]
#define CURRENT_MODE_MQTT myConfig.MODES_MQTT_NAME[myConfig.get()->mode]

#define CURRENT_HOLD_STR myConfig.HOLDS_NAME[thermostat.getHold()]
#define CURRENT_HOLD_MQTT myConfig.HOLDS_MQTT_NAME[thermostat.getHold()]
#define CURRENT_HOLD myConfig.get()->away ? (myConfig.get()->awayMode == Config::AWAY_MODES::AS_AUTO ? myConfig.get()->hold : myConfig.get()->awayMode) \
                                          : myConfig.get()->hold

#define CURRENT_ACTION_MQTT at8gw.getRelay() ? "heating" : ((myConfig.get()->mode == Config::MODES::OFF) ? "off" : "idle"); // idle, cooling, heating, drying, or off
*/
class WaterLevel: public EvoStopable
{
public:
    typedef void (*PumpCallback)(bool isPumping);

    WaterLevel();

    void setPumpCallback(PumpCallback heatingCallback) { _heatingCallback = heatingCallback; }

    byte getMode() { return myConfig.get()->mode; }
    void setMode(Config::MODES mode)
    {
        myConfig.get()->mode = mode;
        if (myConfig.get()->mode == Config::MANUAL)
        {
            _manualTime = millis();
        }
        myConfig.saveConfig();
    }

    bool isLevelLow() { return digitalRead(LEVEL_PIN) == LOW; }

    void setPump(bool onoff);
    bool isPumpOn();

    float getActualFlow() { return flowRate; }

    unsigned long getTotalMl() { return totalml; }

    bool start();
    bool stop();
    

private:
    // Fluxometer
    volatile byte pulseCount;
    const float calibrationFactor = 7055; // Hz Spec https://italian.alibaba.com/product-detail/sen-hz41wa-electronic-water-flowmeter-magnetic-flow-sensor-water-flow-sensor-60583976123.html
    
    static void interrupt_handler_static(void* arg);
    void pulseCounter();

    float flowRate;
    unsigned long totalml;
    bool pumpOn = false;

    /*
    void setPump(Config::HOLDS hold)
    {
        myConfig.get()->hold = hold;
        myConfig.get()->mode = Config::MANUAL;
        _manualTime = millis();
        myConfig.saveConfig();
    }

    float getCurrentTarget() { return (myConfig.get()->targetTemp[CURRENT_HOLD] + (myConfig.get()->away ? myConfig.get()->awayModify : 0)); }
    void setCurrentTarget(float temp)
    {
        myConfig.get()->targetTemp[myConfig.get()->hold] = temp - (myConfig.get()->away ? myConfig.get()->awayModify : 0);
        myConfig.saveConfig();
    }
    byte getHold() { return CURRENT_HOLD; }
    void setAway(bool away)
    {
        myConfig.get()->away = away;
        myConfig.saveConfig();
    }
*/

private:
    PumpCallback _heatingCallback = NULL;
    unsigned long _switchLastTime, _manualTime;
    Ticker _loopTicker;
    //    float _curTemp = 20.4f;

    void _loopWaterLevel();
};

extern WaterLevel waterLevel;
