#include "HID.h"
#include "Constants.h"

void IRAM_ATTR HID::interrupt_handler_static(void *arg)
{
    reinterpret_cast<HID *>(arg)->buttonPressed();
}

void IRAM_ATTR HID::buttonPressed()
{
    //    debugD("Pressed Button");
    lastAction = millis();
    status = 0xFF & (!digitalRead(BUTTON1_PIN) << BUTTON_SHIFT[0] |
                     !digitalRead(BUTTON2_PIN) << BUTTON_SHIFT[1] |
                     !digitalRead(BUTTON3_PIN) << BUTTON_SHIFT[2]);
}

HID::HID()
{
    // Buttons
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    pinMode(BUTTON3_PIN, INPUT_PULLUP);

    attachInterruptArg(digitalPinToInterrupt(BUTTON1_PIN), &HID::interrupt_handler_static, this, FALLING);
    attachInterruptArg(digitalPinToInterrupt(BUTTON2_PIN), &HID::interrupt_handler_static, this, FALLING);
    attachInterruptArg(digitalPinToInterrupt(BUTTON3_PIN), &HID::interrupt_handler_static, this, FALLING);

    lastAction = millis();
}

HID::BSTATUS HID::getButton(byte button)
{
    byte cstatus = !digitalRead(BUTTON_PIN[button - 1]) << BUTTON_SHIFT[button - 1];
    if (cstatus)
    {
        //debugD("Bt:%d ST:" BYTE_TO_BINARY_PATTERN " CS:" BYTE_TO_BINARY_PATTERN, button, BYTE_TO_BINARY(status), BYTE_TO_BINARY(cstatus));
        if (millis() - lastAction > 1000)
        { // Hold Button Press over 700ms
            BSTATUS ret = (cstatus & (status & (1 << BUTTON_SHIFT[button - 1]))) ? HID::BSTATUS::HOLD : HID::BSTATUS::NONE;
        //    debugD("Hold button s:" BYTE_TO_BINARY_PATTERN " ret %d", BYTE_TO_BINARY(status), ret);
            return ret;
        } 
        else if (millis() - lastAction > 700)
        { // Long Button Press 700ms
            BSTATUS ret = (cstatus & (status & (1 << BUTTON_SHIFT[button - 1]))) ? HID::BSTATUS::LONG_PRESS : HID::BSTATUS::NONE;
        //    debugD("Long press button s:" BYTE_TO_BINARY_PATTERN " ret %d", BYTE_TO_BINARY(status), ret);
            return ret;
        }
        else
        {
            return cstatus ? HID::BSTATUS::PRESS : HID::BSTATUS::NONE;
        }
    }
    return HID::BSTATUS::NONE;
}

bool HID::getCombo12()
{
    return getButton(1) && getButton(2);
}
bool HID::getCombo13()
{
    return getButton(1) && getButton(3);
}
bool HID::getCombo23()
{
    return getButton(2) && getButton(3);
}
bool HID::getAny123()
{
    return getButton(1) || getButton(2) || getButton(2);
}

HID hid = HID();