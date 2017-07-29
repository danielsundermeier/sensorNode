#include <Arduino.h>
#include <ESP8266mDNS.h>
#include "config/all.h"

char wiFiHostname[ ] = "SensorNode";

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
    Serial.println("Booting");

    // PIN MODE

    // SPIFFS
    // SPIFFS.begin();
}

void software_Reset() {
    Serial.print("resetting");
    ESP.reset(); 
}

// -----------------------------------------------------------------------------
// BOOTING
// -----------------------------------------------------------------------------

void setup() {
    
    hardwareSetup();
    settingsSetup();
    Serial.print("Hostname: "); 
    if (getSetting("hostname").length() == 0) {
        setSetting("hostname", getIdentifier());
        saveSettings();
    }
    else {
        Serial.print("aus Speicher ");
    }
    Serial.println(getSetting("hostname"));
    
    sensorsSetup();
    if (wifiSetup()) {
        mdnsSetup(); // hostname.local
        otaSetup();
        mqttSetup();
    }
}

void loop() {

    sensorsLoop();
    mqttLoop();
    webLoop();
    otaLoop();
    
}