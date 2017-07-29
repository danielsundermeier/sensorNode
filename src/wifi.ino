/*
 *
WIFI MODULE

Copyright (C)  by Daniel Sundermeier <daniel at hof-sundermeier dot de>

*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

String wifiNetworks[WIFI_MAX_NETWORKS];
String wifiNetwork[3];

int numNetworks;
int wifiSelected;

// -----------------------------------------------------------------------------
// WIFI
// -----------------------------------------------------------------------------

// TODO: wifiNetworks mit verfügbaren Netzwerken füllen
// Nach WiFi.RSSI sortieren
int wifiScan() {
    int selected  = -1;
    
    numNetworks   = WiFi.scanNetworks();

    if (numNetworks == -1) {
        Serial.println( "Keine Wifi Netzwerke gefunden" );
        return selected;
    }

    Serial.print( "Anzahl verfügbarer Netzwerke: " );
    Serial.println( numNetworks );

    Serial.println ( "Netzwerk Liste" );

    for (int thisNet = 0; thisNet<numNetworks; thisNet++) {
        Serial.print( thisNet );
        Serial.print( ") SSID: " );
        Serial.print( WiFi.SSID(thisNet) );
        Serial.print( "\tSignal: " );
        Serial.print( WiFi.RSSI(thisNet) );
        Serial.println( " dBm" );
        for ( int i=0; i< (WIFI_MAX_NETWORKS -1) && selected == -1 ; i++ ) {
            if ( wifiNetworksDefault[i][0]==WiFi.SSID(thisNet)) {
                selected=i;
                break;
            }
        }
    }
    if ( selected >= 0 ) {
        Serial.println ( "\nErkanntes Netzwerk SSID: " + String ( wifiNetworksDefault[selected][0] ) );
    } else {
        Serial.println ( "\nKeine bekannten Netzwerke gefunden" );
    }

    return selected;
}

// TODO: Checken, ob ein verfügbares WLAN in wifiNetworks ist
// versuchen zu verbinden, wenn verbinden erfolgreich
// key zurückgegeben
int wifiSelect() {

}

bool wifiSetup() {
    char ssid[50];
    char password[50];

    Serial.println( "WLAN Setup..." );

    int selectedWifiKey = wifiScan();
    if (selectedWifiKey >= 0) {
        wifiNetworksDefault[selectedWifiKey][0].toCharArray(ssid, 50);
        wifiNetworksDefault[selectedWifiKey][1].toCharArray(password, 50);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        WiFi.hostname(getSetting("hostname"));
        Serial.print( "Verbinden" );
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print( "IP-Adresse: " );
        Serial.println(getIP());

        return true;
    }
    else {
        Serial.println( "Kein bekanntes Netzwerk gefunden" );
        
        return false;
    }
}

String getIP() {
    return WiFi.localIP().toString();
}

String getSSID() {
    return WiFi.SSID();
}
