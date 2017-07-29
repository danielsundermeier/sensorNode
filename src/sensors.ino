/*
 *
Sensors MODULE

Copyright (C)  by Daniel Sundermeier <daniel at hof-sundermeier dot de>

*/

#include <DHT.h>

DHT dht(DHTPIN, DHTTYPE);

float ldrValue;
int LDR;
float calcLDR;
float diffLDR = 25;

float diffTEMP = 0.2;
float tempValue;

float diffHUM = 1;
float humValue;

int pirValue;
int pirStatus;
String motionStatus;

byte red = 255;
byte green = 255;
byte blue = 255;
byte brightness = 255;

byte realRed = 0;
byte realGreen = 0;
byte realBlue = 0;

bool stateOn = false;

// -----------------------------------------------------------------------------
// Sensors
// -----------------------------------------------------------------------------

void sensorsSetup() {
	pinMode(DHTPIN, INPUT);
    pinMode(LDRPIN, INPUT);
}

void sensorsLoop() {
	float newTempValue = getTempValue();
	float newHumValue = getHumValue();

	if (checkBoundSensor(newTempValue, tempValue, diffTEMP)) {
        tempValue = newTempValue;
        sendState();
    }

    if (checkBoundSensor(newHumValue, humValue, diffHUM)) {
        humValue = newHumValue;
        sendState();
	}

    int newLDR = getLdrValue();

    if (checkBoundSensor(newLDR, LDR, diffLDR)) {
        LDR = newLDR;
        sendState();
    }
}

float getHumValue() {
	return dht.readHumidity();
}

float getTempValue() {
	return dht.readTemperature();
}

int getLdrValue() {
    return analogRead(LDRPIN);
}

float calculateHeatIndex(float humidity, float temp) {
    float heatIndex= 0;
    if (temp >= 80) {
	    heatIndex = -42.379 + 2.04901523*temp + 10.14333127*humidity;
	    heatIndex = heatIndex - .22475541*temp*humidity - .00683783*temp*temp;
	    heatIndex = heatIndex - .05481717*humidity*humidity + .00122874*temp*temp*humidity;
	    heatIndex = heatIndex + .00085282*temp*humidity*humidity - .00000199*temp*temp*humidity*humidity;
  	} else {
    	heatIndex = 0.5 * (temp + 61.0 + ((temp - 68.0)*1.2) + (humidity * 0.094));
    }

    if (humidity < 13 && 80 <= temp <= 112) {
        float adjustment = ((13-humidity)/4) * sqrt((17-abs(temp-95.))/17);
        heatIndex = heatIndex - adjustment;
    }

    return heatIndex;
}

bool checkBoundSensor(float newValue, float prevValue, float maxDiff) {
  	return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}
