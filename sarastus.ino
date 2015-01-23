#include <TM1637Display.h>

const int debugMode = false;

// pins
const int beepPin = 13;
const int ledPin = 11;
const int buttonPin = 2;
const int dispClockPin = 3;
const int dispDioPin = 4;

TM1637Display dd(dispClockPin, dispDioPin);


// brightness
int brightness = 0;
const int min = 0;
const int minVisible = 16;
const int max = 255;
int nightBrightness = max / 10;

// timing state
const int maxCount = 20;
int countToStart = 0;
unsigned long wakeUpTime = 0;
unsigned long hourMs = 3600 * 1000;
const unsigned long countStepMs = debugMode ? 1000 : hourMs;
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
  dd.setBrightness(0);
  dd.setColon(true);
  dd.showNumberDec(countToStart*100, false, 4, 0);
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
      if (millis() >= wakeUpTime) {
        state = BRIGHTENING;
        wakeUpTime += (hourMs * 24);
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
      fadeBrightness(0);
      state = COUNTING_DOWN;
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
  if (brightness > 0) {
     // setting brightness
  } else {
    switch (state) {
      case SETTING_TIMER:
        resetTimer();
        break;
      default:
        state = SETTING_TIMER;
        highBeep();
        if (countToStart == 0) {
          countUp();      
        }
        showCountdown();
        break;
    }  
  }
}

void longPressUp() {
}

void longPressHold() {
  if (brightness > 0) {
    if (brightness == max) {
      setNightBrightness(minVisible);
    } else {
      setNightBrightness(brightness+1);
    }        
  }
}

void beenAWhileSinceButtonPressed() {
  switch (state) {
    case SETTING_TIMER:
      state = COUNTING_DOWN;
      lowBeep();
      highBeep();
      hideCountdown();
      scheduleWakeUp(countToStart);
      break;
  }
}

// helpers

void scheduleWakeUp(int count) {
  wakeUpTime = millis() + (countStepMs * count);
}

void setNightBrightness(int b) {
  nightBrightness = b;
  setBrightness(b);
}

void showDimLights() {
  fadeBrightness(nightBrightness);
}

void resetTimer() {
  highBeep();
  lowBeep();
  state = ZERO;
  countToStart = 0;
  hideCountdown();
}

void setCountdown(int count) {
  countToStart = count;
  showCountdown();
}

void showCountdown() {
  dd.setBrightness(8);
  unsigned long msToStart = (countStepMs * countToStart);
  unsigned long hours = msToStart / hourMs;
  unsigned long minutes = msToStart % hourMs;
  
  dd.showNumberDec(((int)hours)*100, false, 4, 0);
}

void hideCountdown() {
  dd.setBrightness(0);
  dd.showNumberDec(countToStart*100, false, 4, 0); // required for brightness to take effect
}

void countUp() {
  setBrightness(0);
  if (countToStart <= maxCount) {
    setCountdown(countToStart+1);
  }
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

void brighten() {
  if (brightness < max) {
    setBrightness((brightness >= minVisible) ? (brightness + 1) : (minVisible));
    nextStep = millis() + riseStepMs;
  }
}

void lowBeep() {
  beep(50, 2);
}

void highBeep() {
  beep(100, 1);
}

void beep(int c, int d) {
  for (int i = 0; i < c; i++) {
    digitalWrite(beepPin, HIGH);
    delay(d);
    digitalWrite(beepPin, LOW);
    delay(d);  
  }
}
