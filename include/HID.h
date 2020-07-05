#pragma once
#include "Config.h"
#include "EvoDebug.h"

class HID
{
public:
    HID();

    enum BSTATUS {
        NONE = 0,
        PRESS,
        LONG_PRESS,
        HOLD
    };

    BSTATUS getButton(byte button);

    bool getCombo12(); 
    bool getCombo13();
    bool getCombo23();
    bool getAny123();

    time_t lastAction;

private: 
    static void interrupt_handler_static(void* arg);
    void buttonPressed();


    byte status = 0x00; 
    const byte BUTTON_SHIFT[3] = {0,1,2};
    const byte BUTTON_PIN[3] = {BUTTON1_PIN,BUTTON2_PIN,BUTTON3_PIN};


};

extern HID hid;