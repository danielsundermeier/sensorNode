/*
 *
WEB MODULE

Copyright (C)  by Daniel Sundermeier <daniel at hof-sundermeier dot de>

*/

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>

ESP8266WebServer server(80);

String html;

// -----------------------------------------------------------------------------
// WEBSERVER
// -----------------------------------------------------------------------------

void webSetup() {
    server.on("/edit", []() {
        html = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        html += getIP();
        html += "<p>";
        html += "</p><form method='post' action='save'>";
        html += "<label>Hostname: </label><input name='hostname' value='"+String(getSetting("hostname").c_str())+"' length=32><br>";
        for(int i = 0;i<WIFI_MAX_NETWORKS;i++) {
            html += "<label>SSID "+String(i)+": </label><input name='ssid"+String(i)+"' length=32><input name='pass"+String(i)+"' length=64><br>";
        }
        html += "<br><input type='submit'></form>";
        html += "</html>";
        server.send(200, "text/html", html); 
    });
    server.on("/save", [](){
        html = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        html += getIP();
        html += "<br>";
        for (int i=0;i<(server.args()-1);i++) {
            html += server.argName(i)+ ": "+server.arg(i)+"<br>"; 
        }
        html = "<br><br><a href='/'>zurück</a>'";

        server.send(200, "text/html", html); 

        setSetting("hostname", server.arg("hostname"));
        saveSettings();
    });
    server.on("/", [](){
        server.send(200, "text/html", "Hello from ESP8266<br><br><a href='/edit'>Einstellungen</a>");
    });

    server.begin();
    Serial.println("TCP server started");
}

void webLoop() {
    server.handleClient();
}
 
// -----------------------------------------------------------------------------
// DNS
// -----------------------------------------------------------------------------

void mdnsSetup() {
    if (MDNS.begin(getSetting("hostname").c_str())) {
        Serial.print("mDNS responder started at ");
        Serial.print(getSetting("hostname").c_str());
        Serial.println(".local");
        webSetup();
        MDNS.addService("http", "tcp", 80);
    }
    else {
        
    }
}