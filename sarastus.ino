int ledPin = 11;
int buttonPin = 2;
int brightness = 0;
int min = 0;
int max = 255;
int maxCount = 7;
int indicatorPins[] = {3,4,5,6,7,8,9};
int countToStart = maxCount;
int countStepMs = 1000; // 1 second
int riseTimeMs = 60000; // 1 minute
int riseStepMs = 1000;
int resetMs = 1000;
unsigned long nextStep = 0;
int zero = false;

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
        longPressDetected();
        isLongPress = true;
      }      
      delay(10);
    }
    if (isLongPress) {
      longPress();
    } else {
      shortPress();
    }
    delay(10);
  }
  
  showCountdown(countToStart);
  if ((!zero) && millis() > nextStep) {
    if (countToStart > 0) {
      setBrightness(min);
      countdown();
    } else if (brightness < max) {
      brighten();
    }
  }
}

void longPressDetected() {
  showCountdown(maxCount);
}

void shortPress() {
  if (brightness == 0) {
    zero = false;
    countUp();
  } else {
    longPress();
  }
}

void longPress() {
  zero = true;  
  showCountdown(0);
  setBrightness(0);
  countToStart = 0;
}

void showCountdown(int count) {
  for (int i = 0; i < maxCount; i++) {
    digitalWrite(indicatorPins[i], (count > i) ? HIGH : LOW);
  }
}

void countUp() {
  setBrightness(0);
  if (countToStart <= maxCount) {
    countToStart++;
  }
  scheduleCountdown();
}


void setBrightness(int b) {
  brightness = b;
  analogWrite(ledPin, brightness);
}

void countdown() {
  countToStart--;
  scheduleCountdown();
}

void scheduleCountdown() {
  nextStep = millis() + countStepMs; 
}

void brighten() {
  setBrightness(brightness + 1);
  nextStep = millis() + riseStepMs;
}
