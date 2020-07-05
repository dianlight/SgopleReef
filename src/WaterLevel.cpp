#include "WaterLevel.h"
#include "EvoDebug.h"

WaterLevel::WaterLevel(){
    // Pump
    pinMode(PUMP_PIN, OUTPUT);

    // Level
    pinMode(LEVEL_PIN, INPUT_PULLDOWN);

    // Fluxometer
    pinMode(FLUX_PIN, INPUT);

}

bool WaterLevel::start(){
    //  _loopTicker.attach_scheduled(5,std::bind(&WaterLevel::_loopWaterLevel, this));
    _loopTicker.attach(
        1, +[](WaterLevel *instance) { instance->_loopWaterLevel(); }, this);

    // Fluxometer
    pulseCount = 0;
    attachInterruptArg(digitalPinToInterrupt(FLUX_PIN), &WaterLevel::interrupt_handler_static, this, CHANGE); // FALLING

    return true;
}

bool WaterLevel::stop(){
    _loopTicker.detach();
    detachInterrupt(digitalPinToInterrupt(FLUX_PIN));
    return true;
}

void IRAM_ATTR WaterLevel::interrupt_handler_static(void *arg)
{
    reinterpret_cast<WaterLevel *>(arg)->pulseCounter();
}

void IRAM_ATTR WaterLevel::pulseCounter()
{
    pulseCount++;
}

void WaterLevel::setPump(bool on)
{
    digitalWrite(PUMP_PIN, on);
    pumpOn = on;
}

bool WaterLevel::isPumpOn(){
    return pumpOn;
}

void WaterLevel::_loopWaterLevel()
{
    if(myConfig.get()->mode == Config::AUTO)
            setPump(isLevelLow());

    // Flux
    static unsigned long currentMillis, previousMillis = 0;
    currentMillis = millis();
    byte pulse1Sec = pulseCount;
    pulseCount = 0;
    totalml += (1000.0 / calibrationFactor) * pulse1Sec;
    flowRate = (1000.0 / calibrationFactor) * (pulse1Sec / ((currentMillis - previousMillis) / 1000.0));
    previousMillis = millis();

    /*
    // Heater control
    if(millis() - _switchLastTime > myConfig.get()->minSwitchTime * 1000){
        // Controllo Switch!
        if(_curTemp - myConfig.get()->tempPrecision < getCurrentTarget()){
            #ifdef DEBUG_EVENT
                debugD("Relay On %f < %f",at8gw.getTemperature() - myConfig.get()->tempPrecision,getCurrentTarget());
            #endif
            if(_heatingCallback != NULL)
                _heatingCallback(true);
            _switchLastTime = millis();
        } else if(_curTemp + myConfig.get()->tempPrecision > getCurrentTarget()) {
            #ifdef DEBUG_EVENT
                debugD("Relay Off %f > %f",at8gw.getTemperature() - myConfig.get()->tempPrecision,getCurrentTarget());
            #endif
            if(_heatingCallback != NULL)
                _heatingCallback(false);
            _switchLastTime = millis();
        }
    }
*/
}

WaterLevel waterLevel;
