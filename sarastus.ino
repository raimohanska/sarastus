int debugMode = false;
int ledPin = 11;
int buttonPin = 2;
int brightness = 0;
int min = 0;
int max = 255;
int maxCount = 7;
int indicatorPins[] = {3,4,5,6,7,8,9};
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
      }      
      delay(10);
    }
    if (!isLongPress) {
      shortPress();
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
  } else {
    zero = false;
    countUp();      
  }
}

void showDimLights() {
  showCountdown(0);
  setBrightness(max / 10);
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
