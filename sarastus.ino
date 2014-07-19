int debugMode = false;
int beepPin = 13;
int ledPin = 11;
int buttonPin = 2;
int brightness = 0;
int min = 0;
int minVisible = 16;
int max = 255;
int nightBrightness = max / 10;
int maxCount = 8;
int indicatorPins[] = {3,4,5,6,7,8,9,10};
int countToStart = 0;
int settingTimer = false;
unsigned long countStepMs = debugMode ? 10000 : 3600000;
unsigned long riseStepMs = debugMode ? 1000 : 6000;
unsigned long resetMs = 1000;
unsigned long nextStep = 0;
unsigned long previousPress = 0;
int zero = true;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(beepPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < maxCount; i++) {
    pinMode(indicatorPins[i], OUTPUT);
  }
}

void loop() {  
  int buttonPressed = (digitalRead(buttonPin) == LOW);
  if (buttonPressed) { 
    previousPress = millis();
    int isLongPress = false;
    while(digitalRead(buttonPin) == LOW) {
      if (!isLongPress && (millis() > previousPress + resetMs)) {
        longPress();
        isLongPress = true;
      } else if (isLongPress) {
        longPressHold();
      }
      delay(10);
    }
    if (!isLongPress) {
      shortPress();
    } else {
      longPressUp();
    }
    delay(10);
  } else if ((previousPress > 0) && (millis() - previousPress > 5000)) {
    previousPress = 0;
    beenAWhileSinceButtonPressed();
  }
  
  if ((!zero) && millis() > nextStep) {
    if (countToStart > 0) {
      countdown();
    } else if (brightness < max) {
      brighten();
    }
  }
  
  if (settingTimer) {
    blinkLeds();
  }
}

void shortPress() {
  if (settingTimer) {
    countUp();
  } else if (zero || (countToStart > 0)) { // normal operation, also during timer
    if (brightness == 0) {
      showDimLights();
    } else {
      setBrightness(0);
    }
  } else { // brightening
    reset();
  }
}

void longPress() {
  if (!zero) {
    reset();
  } else if (brightness == 0) {
    zero = false;
    settingTimer = true;
    countUp();      
  }
}

void longPressUp() {
}

void longPressHold() {
  if (zero && brightness > 0) {
    if (brightness == max) {
      setNightBrightness(minVisible);
    } else {
      setNightBrightness(brightness+1);
    }
  }
}

void beenAWhileSinceButtonPressed() {
  settingTimer = false;
  showCountdown(countToStart);
}

void setNightBrightness(int b) {
  nightBrightness = b;
  setBrightness(b);
}

void showDimLights() {
  setBrightness(nightBrightness);
}

void reset() {
  zero = true;
  settingTimer = false; 
  showCountdown(0);
  setBrightness(0);
}

void showCountdown(int count) {
  countToStart = count;
  for (int i = 0; i < maxCount; i++) {
    digitalWrite(indicatorPins[i], (count > i) ? HIGH : LOW);
  }
}

void blinkLeds() {
  int on = ((millis() / 500) % 3 > 0);
  
  for (int i = 0; i < maxCount; i++) {
    digitalWrite(indicatorPins[i], ((i == countToStart -1) ? on : (countToStart > i)) ? HIGH : LOW);
  }

}

void countUp() {
  zero = false;
  setBrightness(0);
  if (countToStart <= maxCount) {
    showCountdown(countToStart+1);
  }
  scheduleCountdown();
}

void setBrightness(int b) {
  brightness = b;
  int b2 = (b * b) / max;
  analogWrite(ledPin, b2);
}

void countdown() {
  showCountdown(countToStart-1);
  if (debugMode) {
    beep();
  }
  if (countToStart > 0) {
    scheduleCountdown();
  }
}

void scheduleCountdown() {
  nextStep = millis() + countStepMs; 
}

void brighten() {
  setBrightness((brightness >= minVisible) ? (brightness + 1) : (minVisible));
  nextStep = millis() + riseStepMs;
}

void beep() {
  for (int i = 0; i < 100; i++) {
    digitalWrite(beepPin, HIGH);
    delay(1);
    digitalWrite(beepPin, LOW);
    delay(1);  
  }
}
