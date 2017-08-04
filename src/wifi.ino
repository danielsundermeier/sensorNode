/*
 *
WIFI MODULE

*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

bool wifiSetup() {
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(180);
    wifiManager.autoConnect();
}

String getIP() {
    return WiFi.localIP().toString();
}

String getSSID() {
    return WiFi.SSID();
}
