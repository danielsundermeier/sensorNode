/*
 *
MQTT MODULE

Copyright (C)  by Daniel Sundermeier <daniel at hof-sundermeier dot de>

*/

#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* on_cmd = "ON";
const char* off_cmd = "OFF";

char message_buff[100];

const int BUFFER_SIZE = 300;

// -----------------------------------------------------------------------------
// MQTT
// -----------------------------------------------------------------------------

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

	sendState();
}

void sendState() {
	StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();

	/*
	root["state"] = (stateOn) ? on_cmd : off_cmd;
	JsonObject& color = root.createNestedObject("color");
	color["r"] = red;
	color["g"] = green;
	color["b"] = blue;
	*/

	float humValue = getHumValue();
	float tempValue = getTempValue();
	int LDR = getLdrValue();

	//root["brightness"] = brightness;
	root["humidity"] = (String)humValue;
	//root["motion"] = (String)motionStatus;
	root["ldr"] = (String)LDR;
	root["temperature"] = (String)tempValue;
	root["heatIndex"] = (String)calculateHeatIndex(humValue, tempValue);


	char buffer[root.measureLength() + 1];
	root.printTo(buffer, sizeof(buffer));

	Serial.println(buffer);
	mqttClient.publish(getSetting("mqtt_topic_stat").c_str(), buffer, true);
}

void mqttconnect() {
	while (!mqttClient.connected()) {
	    Serial.print("Attempting MQTT connection...");
	    // Attempt to connect
	    if (mqttClient.connect(getSetting("hostname").c_str(), getSetting("mqtt_user").c_str(), getSetting("mqtt_password").c_str())) {
	        Serial.println("connected");
	        mqttClient.subscribe(MQTT_TOPIC_CMND);
	        //setColor(0, 0, 0);
	        sendState();
	    } else {
	        Serial.print("failed, rc=");
	        Serial.print(mqttClient.state());
	        Serial.println(" try again in 5 seconds");
	        // Wait 5 seconds before retrying
	        delay(5000);
	    }
	}
}

void mqttSetup() {
	int Parts[4] = {0,0,0,0};
	int Part = 0;
	for ( int i=0; i<getSetting("mqtt_server").length(); i++ ) {
		char c = getSetting("mqtt_server")[i];
		if ( c == '.' ) {
			Part++;
			continue;
		}
		Parts[Part] *= 10;
		Parts[Part] += c - '0';
	}
	IPAddress mqttServer( Parts[0], Parts[1], Parts[2], Parts[3] );
	Serial.print("String: ");
	Serial.println(getSetting("mqtt_server"));
	Serial.print("IP: ");
	Serial.println(mqttServer);
	mqttClient.setServer(mqttServer, getSetting("mqtt_port").toInt());
	mqttClient.setCallback(callback);
	mqttconnect();
}

void mqttLoop() {
	if (!mqttClient.connected()) {
	    mqttconnect();
	}
	mqttClient.loop();
}