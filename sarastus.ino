const int debugMode = false;

// pins
const int beepPin = 13;
const int ledPin = 11;
const int buttonPin = 2;
const int indicatorPins[] = {3,4,5,6,7,8,9,10};

// brightness
int brightness = 0;
const int min = 0;
const int minVisible = 16;
const int max = 255;
int nightBrightness = max / 10;

// timing state
const int maxCount = 8;
int countToStart = 0;
const unsigned long countStepMs = debugMode ? 1000 : 3600000;
const unsigned long riseStepMs = debugMode ? 1000 : 6000;
const unsigned long resetMs = 1000;
unsigned long nextStep = 0;

// app state
const int ZERO = 0;
const int COUNTING_DOWN = 1;
const int BRIGHTENING = 2;
const int SETTING_TIMER = 3;

int state = ZERO;

// button housekeeping
unsigned long previousPress = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(beepPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  for (int i = 0; i < maxCount; i++) {
    pinMode(indicatorPins[i], OUTPUT);
  }
}

void loop() {  
  checkButtonState();
  advanceAppState();
}

void checkButtonState() {
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
}

void advanceAppState() {
  switch (state) {
    case COUNTING_DOWN:
      if (millis() > nextStep) {
        countdown();
      }
      break;
    case BRIGHTENING:
      if (millis() > nextStep) {
        brighten();
      }
      break;
  }
}

// UI handlers

void shortPress() {
  switch (state) {
    case SETTING_TIMER:
      countUp();
      break;
    case BRIGHTENING:
      reset();
      break;
    default:
      if (brightness == 0) {
        showDimLights();
      } else {
        fadeBrightness(0);
      }
      break;
  }
}

void longPress() {
  switch (state) {
    case ZERO:
      if (brightness == 0) {
        state = SETTING_TIMER;
        countUp();      
      }
      break;
    default:
      reset();
      break;
  }  
}

void longPressUp() {
}

void longPressHold() {
  switch (state) {
    case ZERO:
      if (brightness > 0) {
        if (brightness == max) {
          setNightBrightness(minVisible);
        } else {
          setNightBrightness(brightness+1);
        }        
      }
      break;
  }  
}

void beenAWhileSinceButtonPressed() {
  switch (state) {
    case SETTING_TIMER:
      state = COUNTING_DOWN;
      showCountdown(0);
      break;
  }
}

// helpers

void setNightBrightness(int b) {
  nightBrightness = b;
  setBrightness(b);
}

void showDimLights() {
  fadeBrightness(nightBrightness);
}

void reset() {
  state = ZERO;
  setCountdown(0);
  setBrightness(0);
}

void setCountdown(int count) {
  countToStart = count;
  showCountdown(count);
}

void showCountdown(int count) {
  for (int i = 0; i < maxCount; i++) {
    digitalWrite(indicatorPins[i], (count > i) ? HIGH : LOW);
  }
}

void countUp() {
  setBrightness(0);
  if (countToStart <= maxCount) {
    setCountdown(countToStart+1);
  }
  scheduleCountdown();
}

void fadeBrightness(int target) {
  int start = max(brightness, minVisible);
  int fadeTime = 500;
  int fadeSteps = abs(target - start);
  int fadeDelay = fadeTime / fadeSteps;
  int fadeStep = (target - start) / fadeSteps;
  
  for (int i = 0; i < fadeSteps; i++) {
    setBrightness(map(i, 0, fadeSteps - 1, start, target));
    delay(fadeDelay);
  }
  setBrightness(target);
}

void setBrightness(int b) {
  brightness = b;
  int b2 = (b * b) / max;
  analogWrite(ledPin, b2);
}

void countdown() {
  countToStart--;
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
  if (brightness < max) {
    setBrightness((brightness >= minVisible) ? (brightness + 1) : (minVisible));
    nextStep = millis() + riseStepMs;
  }
}

void beep() {
  for (int i = 0; i < 100; i++) {
    digitalWrite(beepPin, HIGH);
    delay(1);
    digitalWrite(beepPin, LOW);
    delay(1);  
  }
}
