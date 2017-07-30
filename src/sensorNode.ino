#include <Arduino.h>
#include <ESP8266mDNS.h>
#include "config/all.h"

char wiFiHostname[ ] = "SensorNodeDEV";

template<typename T> bool setSetting(const String& key, T value);
template<typename T> String getSetting(const String& key, T defaultValue);

String getIdentifier() {
    char identifier[20];
    sprintf(identifier, "%s_%06X", DEVICE, ESP.getChipId());
    return String(identifier);
}

void hardwareSetup() {
    // SERIAL
    Serial.begin(SERIAL_BAUDRATE);
    delay(1000);
    Serial.println("Booting");

    // PIN MODE

    // SPIFFS
    // SPIFFS.begin();
}

void software_Reset() {
    Serial.print("resetting");
    ESP.reset(); 
}

void restart() {
    Serial.println("restarting..");
    ESP.restart();
}

// -----------------------------------------------------------------------------
// BOOTING
// -----------------------------------------------------------------------------

void setup() {
    
    hardwareSetup();
    settingsSetup();
    sensorsSetup();
    wifiSetup();
    webSetup();
    otaSetup();
    mqttSetup();
    
}

void loop() {

    sensorsLoop();
    webLoop();
    mqttLoop();
    otaLoop();
    
}