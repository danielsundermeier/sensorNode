/*
 *
WEB MODULE

*/

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>

ESP8266WebServer server(80);

const char SERVER_HTTP_HEAD[] PROGMEM  = "<!DOCTYPE html><html lang=\"de\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char SERVER_HTTP_STYLE[] PROGMEM = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char SERVER_HTTP_HEAD_END[] PROGMEM = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char SERVER_HTTP_END[] PROGMEM = "</div></body></html>";

String html;

// -----------------------------------------------------------------------------
// WEBSERVER
// -----------------------------------------------------------------------------

void webSetup() {
    server.on("/edit", []() {
        html = FPSTR(SERVER_HTTP_HEAD);
        html.replace("{v}", "Einstellungen");
        html += FPSTR(SERVER_HTTP_STYLE);
        html += FPSTR(SERVER_HTTP_HEAD_END);

        html += "<p>";
        html += "<h1>Einstellungen</h1>";
        html += "</p><form method='post' action='save'>";
        html += "<h2>Allgemein</h2>";
        html += "<label>Hostname</label><input name='hostname' type='text' value='"+String(getSetting("hostname").c_str())+"'><br>";
        html += "<h2>MQTT</h2>";
        html += "<label>Server</label><input name='mqtt_server' type='text' value='"+String(getSetting("mqtt_server").c_str())+"'><br>";
        html += "<label>User</label><input name='mqtt_user' type='text' value='"+String(getSetting("mqtt_user").c_str())+"'><br>";
        html += "<label>Passwort</label><input name='mqtt_password' type='password' value='"+String(getSetting("mqtt_password").c_str())+"'><br>";
        html += "<label>Port</label><input name='mqtt_port' type='text' value='"+String(getSetting("mqtt_port").c_str())+"'><br>";
        html += "<label>Topic Status</label><input name='mqtt_stat' type='text' value='"+String(getSetting("mqtt_stat").c_str())+"'><br>";
        html += "<label>Topic Command</label><input name='mqtt_cmnd' type='text' value='"+String(getSetting("mqtt_cmnd").c_str())+"'><br>";
        html += "<br><input type='submit'></form>";

        html += FPSTR(SERVER_HTTP_END);
        server.send(200, "text/html", html); 
    });
    server.on("/save", [](){
        html = FPSTR(SERVER_HTTP_HEAD);
        html.replace("{v}", "Speichern");
        html += FPSTR(SERVER_HTTP_STYLE);
        html += FPSTR(SERVER_HTTP_HEAD_END);

        for (int i=0;i<(server.args()-1);i++) {
            html += server.argName(i)+ ": "+server.arg(i)+"<br>"; 
        }
        html += "<br><br><a href='/'>zurück</a>'";

        html += FPSTR(SERVER_HTTP_END);

        server.send(200, "text/html", html); 
        for (int i=0;i<(server.args()-1);i++) {
            if (getSetting(server.argName(i)) != server.arg(i)) {
                setSetting(server.argName(i), server.arg(i));
            }
        }
        //setSetting("hostname", server.arg("hostname"));
        saveSettings();
        restart();
    });
    server.on("/", [](){
        html = FPSTR(SERVER_HTTP_HEAD);
        html.replace("{v}", "Start");
        html += FPSTR(SERVER_HTTP_STYLE);
        html += FPSTR(SERVER_HTTP_HEAD_END);

        html += "<h1>Start</h1>";
        html += "Hello from ESP8266<br><br><a href='/edit'>Einstellungen</a>";

        html += FPSTR(SERVER_HTTP_END);

        server.send(200, "text/html", html);
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