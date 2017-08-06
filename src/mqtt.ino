/*
 *
MQTT MODULE

The MQTT_MAX_PACKET_SIZE parameter may not be setting appropriately do to a bug in the PubSub library. 
If the MQTT messages are not being transmitted as expected please you may need to change the MQTT_MAX_PACKET_SIZE parameter in "PubSubClient.h" directly.
*/

#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

/**************************** PIN DEFINITIONS ********************************************/
const int redPin 			= D0;
const int greenPin 			= D1;
const int bluePin 			= D2;

/**************************** VARS           ********************************************/

const char* on_cmd 			= "ON";
const char* off_cmd 		= "OFF";

char message_buff[100];

const int BUFFER_SIZE 		= 300;

long lastReconnectAttempt 	= 0;

/******************************** GLOBALS for fade/flash *******************************/
byte red 					= 255;
byte green 					= 255;
byte blue 					= 255;
byte brightness 			= 255;

byte flashRed 				= red;
byte flashGreen 			= green;
byte flashBlue 				= blue;
byte flashBrightness 		= brightness;



// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

void mqttDisconnect() {
	mqttClient.disconnect();
}

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");

	char message[length + 1];
	for (int i = 0; i < length; i++) {
	    message[i] = (char)payload[i];
	}
	message[length] = '\0';
	Serial.println(message);

	if (!processJson(message)) {
	    return;
	}

	if (getStateOn()) {
	    // Update lights
	    setRealRed(map(red, 0, 255, 0, brightness));
	    setRealGreen(map(green, 0, 255, 0, brightness));
	    setRealBlue(map(blue, 0, 255, 0, brightness));
	}
	else {
	    setRealRed(0);
	    setRealRed(0);
	    setRealRed(0);
	}

	setStartFade(true);
	setInFade(false); // Kill the current fade

	sendState();
}

void sendState() {
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();

	root["state"] = (getStateOn()) ? on_cmd : off_cmd;
	JsonObject& color = root.createNestedObject("color");
	color["r"] = red;
	color["g"] = green;
	color["b"] = blue;

	float humValue = getHumValue();
	float tempValue = getTempValue();
	int LDR = getLdrValue();


	root["brightness"] = brightness;
	root["humidity"] = (String)humValue;
	root["motion"] = (String)getMotionStatus();
	root["ldr"] = (String)LDR;
	root["temperature"] = (String)tempValue;
	root["heatIndex"] = (String)calculateHeatIndex(humValue, tempValue);


	char buffer[root.measureLength() + 1];
	root.printTo(buffer, sizeof(buffer));

	Serial.println(buffer);
	int published = mqttClient.publish(settings.mqtt_stat.c_str(), buffer, true);
	if (published == 0) {
		Serial.println("Daten konnten nicht gesendet werden.");
	}
}

/********************************** START MQTT CONNECT*****************************************/
void mqttconnect() {
	long now = millis();
	if (now - lastReconnectAttempt > (MQTT_RECONECT_TIMEOUT * 1000)) {
	    lastReconnectAttempt = now;
	    if (mqttClient.connect(settings.hostname.c_str(), settings.mqtt_user.c_str(), settings.mqtt_password.c_str())) {
		    lastReconnectAttempt = 0;
		    Serial.println("connected");
		    if (settings.mqtt_cmnd.length() > 0) mqttClient.subscribe(settings.mqtt_cmnd.c_str());
		    setColor(0, 0, 0);
		    sendState();
	    } else {
	        Serial.print("failed, rc=");
	        Serial.print(mqttClient.state());
	    }
	}
}

/********************************** START PROCESS JSON*****************************************/
bool processJson(char* message) {
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(message);

    if (!root.success()) {
	    Serial.println("parseObject() failed");
	    return false;
    }

	if (root.containsKey("state")) {
	    if (strcmp(root["state"], on_cmd) == 0) {
	        setStateOn(true);
	    }
	    else if (strcmp(root["state"], off_cmd) == 0) {
	        setStateOn(false);
	    }
    }

	// If "flash" is included, treat RGB and brightness differently
	if (root.containsKey("flash")) {
	    setFlashLength((int)root["flash"] * 1000);

	    if (root.containsKey("brightness")) {
	        flashBrightness = root["brightness"];
	    }
	    else {
	        flashBrightness = brightness;
	    }

	    if (root.containsKey("color")) {
	        flashRed = root["color"]["r"];
	        flashGreen = root["color"]["g"];
	        flashBlue = root["color"]["b"];
	    }
	    else {
	        flashRed = red;
	        flashGreen = green;
	        flashBlue = blue;
	    }

	    flashRed = map(flashRed, 0, 255, 0, flashBrightness);
	    flashGreen = map(flashGreen, 0, 255, 0, flashBrightness);
	    flashBlue = map(flashBlue, 0, 255, 0, flashBrightness);

	    setFlash(true);
	    setStartFlash(true);
    }
    else { // Not flashing
	    setFlash(false);

	    if (root.containsKey("color")) {
	        red = root["color"]["r"];
	        green = root["color"]["g"];
	        blue = root["color"]["b"];
	    }

	    if (root.containsKey("brightness")) {
	        brightness = root["brightness"];
	    }

	    if (root.containsKey("transition")) {
	        setTransitionTime(root["transition"]);
	    }
	    else {
	        setTransitionTime(0);
	    }
    }

    return true;
}

/********************************** START SET COLOR *****************************************/
void setColor(int inR, int inG, int inB) {
	analogWrite(redPin, inR);
	analogWrite(greenPin, inG);
	analogWrite(bluePin, inB);

	Serial.println("Setting LEDs:");
	Serial.print("r: ");
	Serial.print(inR);
	Serial.print(", g: ");
	Serial.print(inG);
	Serial.print(", b: ");
	Serial.println(inB);
}

void mqttSetup() {
	int Parts[4] = {0,0,0,0};
	int Part = 0;
	for ( int i=0; i<settings.mqtt_server.length(); i++ ) {
		char c = settings.mqtt_server[i];
		if ( c == '.' ) {
			Part++;
			continue;
		}
		Parts[Part] *= 10;
		Parts[Part] += c - '0';
	}
	IPAddress mqttServer( Parts[0], Parts[1], Parts[2], Parts[3] );
	mqttClient.setServer(mqttServer, settings.mqtt_port.toInt());
	mqttClient.setCallback(callback);
	mqttconnect();
}

void mqttLoop() {
	if (!mqttClient.connected()) {
	    mqttconnect();
	} 
	else {
		mqttClient.loop();
	}
}