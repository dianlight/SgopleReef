#include "Config.h"
#include <Arduino.h>
#include "EvoDebug.h"

static Config::StoreStruct storage;

Config::Config() {}

void Config::init()
{
  if (prefs.begin("config_"))
  {
    size_t ln = prefs.getBytes("config", &storage, sizeof(Config::StoreStruct));

    if (ln == 0){
      debugW("New Configuration found! Write default!");
      prefs.putBytes("config", &storage, sizeof(Config::StoreStruct));
    } else if (storage.version[0] != CONFIG_VERSION[0] ||
        storage.version[1] != CONFIG_VERSION[1] ||
        storage.version[2] != CONFIG_VERSION[2])
    {
      debugW("\nConfig Version UNMATCH '%s' != '%s'!\n", CONFIG_VERSION, storage.version);
    }
    else if (storage.configSize != sizeof(Config::StoreStruct))
    {
      debugE("Illegal Config Size %d != %d!", storage.configSize, sizeof(Config::StoreStruct));
    }
    _started = true;
    debugI("Config Loaded!");
  }
  else
  {
    delay(1000);
    debugE("failed to initialize Properties of size %d", sizeof(Config::StoreStruct));
  }
}

void Config::saveConfig()
{
  if(_started)
    prefs.putBytes("config", &storage, sizeof(Config::StoreStruct));
  else {
    debugE("Config save before config.init!");  
    throw "Config save before config.init!"; 
  }
}

Config::StoreStruct *Config::get()
{
  if(_started)
    return &storage;
  else {
    debugE("Config get before config.init!");  
    throw "Config get before config.init!";
  }
}

Config myConfig = Config();
