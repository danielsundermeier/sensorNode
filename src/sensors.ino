/*
 *
Sensors MODULE

*/

#include <DHT.h>

DHT dht(DHTPIN, DHTTYPE);

/**************************** SENSOR DEFINITIONS *******************************************/
float ldrValue;
int LDR;
float calcLDR;
float diffLDR                 = 25;

float diffTEMP                = 0.2;
float tempValue;

float diffHUM                 = 1;
float humValue;

int pirValue;
int pirStatus;
String motionStatus;

byte realRed                  = 0;
byte realGreen                = 0;
byte realBlue                 = 0;

bool stateOn                  = false;

bool startFade                = false;
unsigned long lastLoop        = 0;
int transitionTime            = 0;
bool inFade                   = false;
int loopCount                 = 0;
int stepR, stepG, stepB;
int redVal, grnVal, bluVal;

bool flash                    = false;
bool startFlash               = false;
int flashLength               = 0;
unsigned long flashStartTime  = 0;


// -----------------------------------------------------------------------------
// Sensors
// -----------------------------------------------------------------------------

void sensorsSetup() {
	  pinMode(PIRPIN, INPUT);
    pinMode(DHTPIN, INPUT);
    pinMode(LDRPIN, INPUT);
}

void sensorsLoop() {

    if (!inFade) {

    	float newTempValue = getTempValue();
    	float newHumValue = getHumValue();
      pirValue = digitalRead(PIRPIN);

      if (pirValue == LOW && pirStatus != 1) {
          motionStatus = "standby";
          sendState();
          pirStatus = 1;
      }
      else if (pirValue == HIGH && pirStatus != 2) {
          motionStatus = "motion detected";
          sendState();
          pirStatus = 2;
      }

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

    if (flash) {
        if (startFlash) {
          startFlash = false;
          flashStartTime = millis();
        }

        if ((millis() - flashStartTime) <= flashLength) {
          if ((millis() - flashStartTime) % 1000 <= 500) {
            setColor(flashRed, flashGreen, flashBlue);
          }
          else {
            setColor(0, 0, 0);
            // If you'd prefer the flashing to happen "on top of"
            // the current color, uncomment the next line.
            // setColor(realRed, realGreen, realBlue);
          }
        }
        else {
          flash = false;
          setColor(realRed, realGreen, realBlue);
        }
      }

      if (startFade) {
        // If we don't want to fade, skip it.
        if (transitionTime == 0) {
          setColor(realRed, realGreen, realBlue);

          redVal = realRed;
          grnVal = realGreen;
          bluVal = realBlue;

          startFade = false;
        }
        else {
          loopCount = 0;
          stepR = calculateStep(redVal, realRed);
          stepG = calculateStep(grnVal, realGreen);
          stepB = calculateStep(bluVal, realBlue);

          inFade = true;
        }
    }

    if (inFade) {
        startFade = false;
        unsigned long now = millis();
        if (now - lastLoop > transitionTime) {
            if (loopCount <= 1020) {
                lastLoop = now;

                redVal = calculateVal(stepR, redVal, loopCount);
                grnVal = calculateVal(stepG, grnVal, loopCount);
                bluVal = calculateVal(stepB, bluVal, loopCount);

                setColor(redVal, grnVal, bluVal); // Write current values to LED pins

                Serial.print("Loop count: ");
                Serial.println(loopCount);
                loopCount++;
            }
            else {
                inFade = false;
            }
        }
    }

}

String getMotionStatus() {
  return motionStatus;
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

bool getStateOn() {
    return stateOn;
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

void setStartFade(bool val) {
    startFade = val;
}

void setInFade(bool val) {
    inFade = val;
}

void setFlash(bool val) {
    flash = val;
}

void setStartFlash(bool val) {
    startFlash = val;
}

void setFlashLength(int val) {
    flashLength = val;
}

void setStateOn(bool val) {
    stateOn = val;
}

void setTransitionTime(int val) {
    transitionTime = val;
}

void setRealRed(byte val) {
    realRed = val;
}
void setRealGreen(byte val) {
    realGreen = val;
}
void setRealBlue(byte val) {
    realBlue = val;
}



/**************************** START TRANSITION FADER *****************************************/
// From https://www.arduino.cc/en/Tutorial/ColorCrossfader
/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
  The program works like this:
  Imagine a crossfade that moves the red LED from 0-10,
    the green from 0-5, and the blue from 10 to 7, in
    ten steps.
    We'd want to count the 10 steps and increase or
    decrease color values in evenly stepped increments.
    Imagine a + indicates raising a value by 1, and a -
    equals lowering it. Our 10 step fade would look like:
    1 2 3 4 5 6 7 8 9 10
  R + + + + + + + + + +
  G   +   +   +   +   +
  B     -     -     -
  The red rises from 0 to 10 in ten steps, the green from
  0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
  In the real program, the color percentages are converted to
  0-255 values, and there are 1020 steps (255*4).
  To figure out how big a step there should be between one up- or
  down-tick of one of the LED values, we call calculateStep(),
  which calculates the absolute gap between the start and end values,
  and then divides that gap by 1020 to determine the size of the step
  between adjustments in the value.
*/
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero,
    step = 1020 / step;          //   divide by 1020
  }

  return step;
}

/* The next function is calculateVal. When the loop value, i,
   reaches the step size appropriate for one of the
   colors, it increases or decreases the value of that color by 1.
   (R, G, and B are each calculated separately.)
*/
int calculateVal(int step, int val, int i) {
  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;
    }
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    }
  }

  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  }
  else if (val < 0) {
    val = 0;
  }

  return val;
}
