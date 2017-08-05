/*
 *
WIFI MODULE

*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

bool wifiSetup() {
	WiFi.persistent(false);
    WiFiManager wifiManager;
    if (WiFi.SSID()) {
    	wifiManager.setConnectTimeout(180);
    }
    wifiManager.autoConnect();
}

String getIP() {
    return WiFi.localIP().toString();
}

String getSSID() {
    return WiFi.SSID();
}
