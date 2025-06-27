#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <LittleFS.h>

#include <EspFileManager.h>

#define ST_SSID "asus"
#define ST_PASS "Aapnootmies2018"

AsyncWebServer server(80);
EspFileManager FileManager;

void setup() 
{
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ST_SSID, ST_PASS);
    while (WiFi.status() != WL_CONNECTED);
    {
        Serial.print(".");
        delay(50);
    }
    Serial.print("Connected to wifi... \nIP: ");
    Serial.println(WiFi.localIP());

    FileManager.initSDCard();
    FileManager.setServer(&server);

    server.begin();
}

void loop() 
{

}