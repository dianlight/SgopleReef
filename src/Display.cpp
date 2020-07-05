#include <Arduino.h>
#include "Display.h"
#include <WiFi.h>
#include "EvoDebug.h"
#include <Wire.h>

#include "Config.h"
#include "WaterLevel.h"
#include "HID.h"

//#define fontName u8g2_font_profont10_tf
#define fontName u8g2_font_haxrcorp4089_tr
#define fontX 8
#define fontY 10
#define offsetX 0
#define offsetY 0

//static time_t lastAction;
static bool psMode = false;

void Display::sleepModeDisplay()
{
  if (millis() - hid.lastAction > myConfig.get()->displayPowerOff * 1000)
  {
    _u8g2.setPowerSave(1);
#ifdef DEBUG_DISPLAY
    debugI("PowerOff Display");
#endif
    psMode = true;
  }
  else if (millis() - hid.lastAction > myConfig.get()->displaySleep * 1000)
  {
    uint8_t ctn = map((millis() - hid.lastAction) / 1000, myConfig.get()->displaySleep, myConfig.get()->displayPowerOff, myConfig.get()->displayContrast / 2, 0);
    _u8g2.setContrast(ctn);
#ifdef DEBUG_DISPLAY
    debugI("Display Power %d", ctn);
#endif
    psMode = true;
  }
  else
  {
    psMode = false;
  }
}

Display::Display() {
  //_u8g2 = U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA); // pin remapping with ESP8266 HW I2C
  _u8g2 = U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C(U8G2_R0,  /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // pin remapping with ESP8266 HW I2C

  _u8g2.begin();
  _u8g2.enableUTF8Print();
  _u8g2.setFont(fontName);

  _u8g2.firstPage();
  do
  {
    _u8g2.drawUTF8(0, 16, "Please wait...");
  } while (_u8g2.nextPage());

}

bool Display::start()
{


  sleepModeTicker.attach(
      myConfig.get()->displaySleep / 3, +[](Display *instance) { instance->sleepModeDisplay(); }, this);
  displayRefreshTicker.attach_ms(
      100, +[](Display *instance) { instance->loopDisplay(); }, this);

  return true;
}

bool Display::stop()
{
  sleepModeTicker.detach();
  displayRefreshTicker.detach();
  return true;
}

//unsigned long lastTime = 0, lastBtnTime = 0;

void Display::loopDisplay()
{
  static time_t lastTime = millis();

  if (hid.getAny123())
  {
    psMode = false;
    _u8g2.setPowerSave(0);
    _u8g2.setContrast(myConfig.get()->displayContrast);
  }

  if (myConfig.get()->mode == Config::MODES::MANUAL && hid.getButton(1) == HID::BSTATUS::LONG_PRESS)
  { 
    myConfig.get()->mode = Config::MODES::AUTO;
    myConfig.saveConfig();
  } 
  else if (hid.getButton(3) == HID::BSTATUS::LONG_PRESS)
  { 
    myConfig.get()->mode = Config::MODES::MANUAL;
    myConfig.saveConfig();
  } 
  else if (myConfig.get()->mode == Config::MODES::MANUAL && hid.getButton(1) == HID::BSTATUS::PRESS)
  {
    waterLevel.setPump(true);
  }
  else if (myConfig.get()->mode == Config::MODES::MANUAL && hid.getButton(3) == HID::BSTATUS::PRESS)
  {
    waterLevel.setPump(false);
  }

  bool blink = (millis() - lastTime) > 800;
  bool fblink = (millis() - lastTime) > 900;
  lastTime = millis();

//  _u8g2.clearBuffer();
  _u8g2.firstPage();
  do
  {
    

    // Header ( Target temp, Humidity and icons )

    if (WiFi.status() == WL_CONNECTED)
    {
      _u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
      _u8g2.drawGlyph(128 - 8, 10, 0x50);
    }
    else if (blink)
    {
      _u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
      _u8g2.drawGlyph(128 - 8, 10, 0x4A);
    }

    if (waterLevel.isLevelLow() && fblink)
    {
      _u8g2.setFont(u8g2_font_open_iconic_gui_1x_t);
      _u8g2.drawGlyph(128 - 17, 10, 0x47);
    }


    if (waterLevel.isPumpOn() && blink)
    {
      _u8g2.setFont(u8g2_font_open_iconic_human_1x_t);
      _u8g2.drawGlyph(128 - 28, 10, 0x41);
    }

    if (myConfig.get()->mode == Config::MODES::AUTO)
    {
      _u8g2.setFont(u8g2_font_open_iconic_play_1x_t);
      _u8g2.drawGlyph(0, 10, 0x45);
    }
    else
    {
      _u8g2.setFont(u8g2_font_open_iconic_play_1x_t);
      _u8g2.drawGlyph(0, 10, 0x44);
    }

    
  

    _u8g2.setFont(fontName);
    _u8g2.setCursor(12, 10);
    _u8g2.printf("%2.1f\xB0\x43", 22.1);
    _u8g2.setCursor(48 + 10, 10);
    _u8g2.printf("%2.0f%ml/s", waterLevel.getActualFlow());

    _u8g2.setFont(u8g2_font_open_iconic_thing_1x_t);
    _u8g2.drawGlyph(48, 10, 0x48);
/*
    // Footer ( Time and Date )
    _u8g2.setFont(fontName);
    _u8g2.setCursor(0, 63);
    if (!myConfig.getMode()->active)
    {
      _u8g2.printf("%s", CURRENT_MODE_STR);
      _u8g2.setCursor(5 * fontX, 63);
      _u8g2.printf("%s", CURRENT_HOLD_STR);
      _u8g2.setCursor(128 - 47, 63);
      _u8g2.printf("%s  %.2d:%.2d", DAYSNAME[timeNTPClient.getNTPTime()->tm_wday].c_str(), timeNTPClient.getNTPTime()->tm_hour, timeNTPClient.getNTPTime()->tm_min);
    }
    else
    {
      _u8g2.printf("%s", "Config");
      _u8g2.setCursor(5 * fontX, 63);
      _u8g2.printf("%s", CONFIG_TARGET_STR);
      //          u8g2.setCursor(128-45,63);
      //          u8g2.printf("%s %.2d:%.2d",DAYSNAME[getNTPTime()->tm_wday].c_str(),getNTPTime()->tm_hour, getNTPTime()->tm_min);
    }

   */

    // Main display
    _u8g2.drawHLine(0, 13, 128);
    _u8g2.drawHLine(0, 32-13, 128);
   /* 
    if (!myConfig.getMode()->active)
    {
      _u8g2.setFont(u8g2_font_profont29_tr);
      _u8g2.setCursor(26, 40);
      _u8g2.printf("%2.1f", at8gw.getTemperature());
      _u8g2.setFont(u8g2_font_5x7_mf);
      _u8g2.drawGlyph(90, 44 - 22, 'o');
      _u8g2.setFont(fontName);
      _u8g2.drawGlyph(90 + 5, 44 - 17, 'C');
    }
    else
    {
      switch (myConfig.getMode()->position)
      {
      case Config::CONFIG_TARGET::ECO_TEMP:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(26, 40);
        myConfig.get()->targetTemp[0] += (myConfig.getMode()->encoder / 5.0);
        myConfig.getMode()->encoder = 0;
        if (fblink)
          _u8g2.printf("%2.1f", myConfig.get()->targetTemp[0]);
        _u8g2.setFont(u8g2_font_5x7_mf);
        _u8g2.drawGlyph(90, 44 - 22, 'o');
        _u8g2.setFont(fontName);
        _u8g2.drawGlyph(90 + 5, 44 - 17, 'C');
        break;
      case Config::CONFIG_TARGET::NORMAL_TEMP:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(26, 40);
        myConfig.get()->targetTemp[1] += (myConfig.getMode()->encoder / 5.0);
        myConfig.getMode()->encoder = 0;
        if (fblink)
          _u8g2.printf("%2.1f", myConfig.get()->targetTemp[1]);
        _u8g2.setFont(u8g2_font_5x7_mf);
        _u8g2.drawGlyph(90, 44 - 22, 'o');
        _u8g2.setFont(fontName);
        _u8g2.drawGlyph(90 + 5, 44 - 17, 'C');
        break;
      case Config::CONFIG_TARGET::CONFORT_TEMP:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(26, 40);
        myConfig.get()->targetTemp[2] += (myConfig.getMode()->encoder / 5.0);
        myConfig.getMode()->encoder = 0;
        if (fblink)
          _u8g2.printf("%2.1f", myConfig.get()->targetTemp[2]);
        _u8g2.setFont(u8g2_font_5x7_mf);
        _u8g2.drawGlyph(90, 44 - 22, 'o');
        _u8g2.setFont(fontName);
        _u8g2.drawGlyph(90 + 5, 44 - 17, 'C');
        break;
      case Config::CONFIG_TARGET::AWAY_DIFF:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(26, 40);
        myConfig.get()->awayModify += (myConfig.getMode()->encoder / 10.0);
        myConfig.getMode()->encoder = 0;
        if (fblink)
          _u8g2.printf("%2.1f", myConfig.get()->awayModify);
        _u8g2.setFont(u8g2_font_5x7_mf);
        _u8g2.drawGlyph(90, 44 - 22, 'o');
        _u8g2.setFont(fontName);
        _u8g2.drawGlyph(90 + 5, 44 - 17, 'C');
        break;
      case Config::CONFIG_TARGET::MANUAL_TO_AUTO_TIME:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(26, 40);
        myConfig.get()->returnAutoTimeout += (myConfig.getMode()->encoder);
        myConfig.getMode()->encoder = 0;
        if (fblink)
          _u8g2.printf("%d", myConfig.get()->returnAutoTimeout);
        _u8g2.setFont(fontName);
        _u8g2.setCursor(90, 44 - 17);
        _u8g2.print("sec");
        break;
      case Config::CONFIG_TARGET::AWAY_HOLDS:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(0, 40);
        myConfig.get()->awayMode = abs((myConfig.get()->awayMode + myConfig.getMode()->encoder) % Config::AWAY_MODES::AM_SIZE);
        myConfig.getMode()->encoder = 0;
        if (fblink)
          _u8g2.printf("%s", myConfig.AWAY_MODES_NAME[myConfig.get()->awayMode]);
        break;
      case Config::CONFIG_TARGET::INFO:
        _u8g2.setFont(fontName);
        _u8g2.drawUTF8(0, 26, "Version:" _SMT_VERSION "." CONFIG_VERSION);
        //            u8g2.drawUTF8(0,36,String(String("IP:")+WiFi.localIP().toString()).c_str());
        break;
      case Config::CONFIG_TARGET::WIFI:
        _u8g2.setFont(fontName);
        _u8g2.drawUTF8(0, 26, String(String("AP:") + WiFi.SSID()).c_str());
        _u8g2.drawUTF8(0, 36, String(String("IP:") + WiFi.localIP().toString()).c_str());
        break;
      case Config::CONFIG_TARGET::FACTORY_RESET:
        _u8g2.setFont(u8g2_font_profont29_tr);
        _u8g2.setCursor(0, 40);
        myConfig.get()->version[0] = (myConfig.getMode()->encoder % 2 == 0) ? CONFIG_VERSION[0] : 'R';
        myConfig.getMode()->doFactoryReset = !(myConfig.getMode()->encoder % 2 == 0);
        if (fblink)
          _u8g2.printf("%s", myConfig.getMode()->doFactoryReset ? "No Reset" : "Reset");
        break;
      default:
        break;
      }
    }
  */
    /*
    // Program
    if ((
            (!myConfig.get()->awayMode && myConfig.get()->mode == Config::AUTO) || (myConfig.get()->awayMode && myConfig.get()->awayMode == Config::AWAY_MODES::AS_AUTO)) &&
        !myConfig.getMode()->active)
    {
      _u8g2.drawHLine(0, 63 - fontY - 1, 128);
      for (u8 i = 1, h = 0; i < 128; i += 4, h++)
      {
        if (h == timeNTPClient.getNTPTime()->tm_hour)
        {
          for (u8 im = i + 1, dm = 0; dm < 4; dm++, im += 9)
          {
            if (dm == floor(timeNTPClient.getNTPTime()->tm_min / 15))
            {
              if (!blink)
              {
                _u8g2.drawFrame(im, 14 + 28, 9, 3 + myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + dm] * 3);
              }
              else
              {
                _u8g2.drawBox(im, 14 + 28, 9, 3 + myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + dm] * 3);
              }
              myConfig.get()->hold = myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + dm];
              im++;
            }
            else
            {
              _u8g2.drawBox(im, 14 + 28, 8, 3 + myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + dm] * 3);
            }
          }
          i += 35;
        }
        else
        {
          _u8g2.drawBox(i, 14 + 28, 3, 3 + (((myConfig.get()->weekProgram->hourQuarterHolds[(h * 4)] * 3) + (myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + 1] * 3) + (myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + 2] * 3) + (myConfig.get()->weekProgram->hourQuarterHolds[(h * 4) + 3] * 3)) / 4));
        }
      }
    }
  */
  } while (_u8g2.nextPage());
}

void Display::bootAPDisplay(String AP)
{
  _u8g2.setFont(fontName);
  _u8g2.clearBuffer();
  _u8g2.firstPage();
  do
  {
    _u8g2.drawUTF8(0, 16, "Wifi Config Mode");
    _u8g2.drawUTF8(0, 26, String(String("AP:") + AP).c_str());
    _u8g2.drawUTF8(0, 63 - fontY, "Use a Wifi device \nto connect and configure!");
  } while (_u8g2.nextPage());
}

void Display::bootConnectedDisplay()
{
  _u8g2.setFont(fontName);
  _u8g2.clearBuffer();
  _u8g2.firstPage();
  do
  {
    _u8g2.drawUTF8(0, 16, String(String("AP:") + WiFi.SSID()).c_str());
    _u8g2.drawUTF8(0, 26, String(String("IP:") + WiFi.localIP().toString()).c_str());
  } while (_u8g2.nextPage());
}

void Display::displayError(String Error)
{
  _u8g2.setPowerSave(0);
  _u8g2.setContrast(myConfig.get()->displayContrast);
  _u8g2.setFont(fontName);
  _u8g2.clearBuffer();
  _u8g2.firstPage();
  do
  {
    _u8g2.drawUTF8(0, 18, Error.c_str());
  } while (_u8g2.nextPage());
}

void Display::clearDisplay()
{
  _u8g2.clear();
};

void Display::displayProgress(uint8_t perc, String type)
{
  _u8g2.setPowerSave(0);
  _u8g2.setContrast(myConfig.get()->displayContrast);
  _u8g2.setFont(fontName);
  _u8g2.clearBuffer();
  _u8g2.firstPage();
  do
  {
    _u8g2.drawUTF8(0, 12, ("OTA " + type + " Update").c_str());
    _u8g2.drawFrame(0,16,128,16);
    _u8g2.setCursor(52, 26);
    _u8g2.printf("%.2d%%", perc);
    _u8g2.setDrawColor(2);
    _u8g2.drawBox(7, 15, perc*126/100,  14);
    //_u8g2.drawRBox(7, 15, perc + 14, 14, 3);
    //_u8g2.drawBox(2,16+2,va_arg(args,int)/va_arg(args,int)*126,14);


  } while (_u8g2.nextPage());
}

static Print *u8g2_print_for_screenshot;

static u8_t u8g2_print_for_screenshot_row, u8g2_print_for_screenshot_bit;

void u8g2_print_bmp_callback(const char *s)
{
  static byte current = 0x00;

  for (u8_t bp = 0; bp < strlen(s); bp++)
  {
    if (u8g2_print_for_screenshot_row == 0 && s[bp] == '1')
    {
      u8g2_print_for_screenshot->print("4"); // Change from P1 to P4 format (Binary)
    }
    else if (u8g2_print_for_screenshot_row == 2 && s[bp] == 0x0A)
    {
      u8g2_print_for_screenshot->print(" "); // Space as separator as BPM P4 Spec
      u8g2_print_for_screenshot_row++;
    }
    else if (u8g2_print_for_screenshot_row < 3)
    {
      u8g2_print_for_screenshot->print(s[bp]);
    }
    else if (s[bp] != 0x0A)
    {
      if (s[bp] == '0')
      {
        current = (current & ~(0x01 << (7 - u8g2_print_for_screenshot_bit))) & 0xFF;
      }
      else
      {
        current = (current | (0x01 << (7 - u8g2_print_for_screenshot_bit))) & 0xFF;
      }
      u8g2_print_for_screenshot_bit++;
      if (u8g2_print_for_screenshot_bit == 8)
      {
        u8g2_print_for_screenshot_bit = 0;
        u8g2_print_for_screenshot->printf("%c", current);
        current = 0x00;
      }
    }
    if (s[bp] == 0x0A)
      u8g2_print_for_screenshot_row++;
  }
}

void Display::screeshotbmp(Print &p)
{
  u8g2_print_for_screenshot = &p;
  u8g2_print_for_screenshot_row = 0;
  u8g2_print_for_screenshot_bit = 0;
  u8g2_WriteBufferPBM(_u8g2.getU8g2(), u8g2_print_bmp_callback);
}

Display display = Display();
