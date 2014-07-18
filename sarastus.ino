int debugMode = false;
int ledPin = 11;
int buttonPin = 2;
int brightness = 0;
int min = 0;
int max = 255;
int nightBrightness = max / 10;
int maxCount = 8;
int indicatorPins[] = {3,4,5,6,7,8,9,10};
int countToStart = 0;
unsigned long countStepMs = debugMode ? 1000 : 60000;
unsigned long riseStepMs = debugMode ? 1000 : 6000;
unsigned long resetMs = 1000;
unsigned long nextStep = 0;
int zero = true;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < maxCount; i++) {
    pinMode(indicatorPins[i], OUTPUT);
  }
  scheduleCountdown();
}

void loop() {  
  int buttonPressed = (digitalRead(buttonPin) == LOW);
  if (buttonPressed) { 
    unsigned long buttonDownTime = millis();
    int isLongPress = false;
    while(digitalRead(buttonPin) == LOW) {
      if (!isLongPress && (millis() > buttonDownTime + resetMs)) {
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
  }
  
  if ((!zero) && millis() > nextStep) {
    if (countToStart > 0) {
      setBrightness(min);
      countdown();
    } else if (brightness < max) {
      brighten();
    }
  }
}

void shortPress() {
  if (zero) {
    if (brightness == 0) {
      showDimLights();
    } else {
      setBrightness(0);
    }
  } else if (brightness == 0) {
    countUp();
  } else {
    longPress();
  }
}

void longPress() {
  if (!zero) {
    reset();
  } else if (brightness == 0) {
    zero = false;
    countUp();      
  }
}

void longPressUp() {
}

void longPressHold() {
  if (zero && brightness > 0) {
    if (brightness == max) {
      setNightBrightness(1);
    } else {
      setNightBrightness(brightness+1);
    }
  }
}

void setNightBrightness(int b) {
  nightBrightness = b;
  setBrightness(b);
}

void showDimLights() {
  showCountdown(0);
  setBrightness(nightBrightness);
}

void reset() {
  zero = true;  
  showCountdown(0);
  setBrightness(0);
}

void showCountdown(int count) {
  countToStart = count;
  for (int i = 0; i < maxCount; i++) {
    digitalWrite(indicatorPins[i], (count > i) ? HIGH : LOW);
  }
}

void countUp() {
  setBrightness(0);
  if (countToStart <= maxCount) {
    showCountdown(countToStart+1);
  }
  scheduleCountdown();
}

void setBrightness(int b) {
  brightness = b;
  analogWrite(ledPin, brightness);
}

void countdown() {
  showCountdown(countToStart-1);
  scheduleCountdown();
}

void scheduleCountdown() {
  nextStep = millis() + countStepMs; 
}

void brighten() {
  setBrightness(brightness + 1);
  nextStep = millis() + riseStepMs;
}
