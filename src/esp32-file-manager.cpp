#include "esp32-file-manager.h"

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

EspFileManager::EspFileManager(/* args */)
{

}

EspFileManager::~EspFileManager()
{
}
