#include <Wire.h>
#include "Adafruit_VL6180X.h"
#include <FastLED.h>
// #include <Scheduler.h>

Adafruit_VL6180X vl = Adafruit_VL6180X();
// const int ledPin = 2;
const int NUM_LEDS = 5;
const int DATA_PIN = 3;
const int DATA_PIN_2 = 4;
const int DATA_PIN_3 = 5;
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS * 2];
CRGB leds3[NUM_LEDS];
const int BRIGHTNESS = 100;
unsigned long previousMillis = 0;
const long interval = 1000;  // second's interval
bool ledState = false;       // current state of second's led
int triggerCount = 0;        // times of second's led
unsigned long stripPreviousMillis = 0;
const long stripInterval = 1000;         // minute's interval
bool stripLit = false;                   // check if led light up
int strip2TriggerCount = 0;              // times of second's led strip
unsigned long strip2PreviousMillis = 0;  // start to record the second led strip
const long strip2Interval = 1000;    // hour's interval

// DC motor
const int motorSpeedPin = 9;    // PWM
const int motorDirection1 = 8;  // D1
const int motorDirection2 = 7;  // D2

// DC motor speed
const int motorSpeed = 255;  // (0-255)

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, DATA_PIN_3, RGB>(leds3, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, DATA_PIN_2>(leds2, NUM_LEDS * 2);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  // Dc motor
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDirection1, OUTPUT);
  pinMode(motorDirection2, OUTPUT);

  while (!Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL6180x test!");
  if (!vl.begin()) {
    Serial.println("Failed to find sensor");
    while (1)
      ;
  }
  Serial.println("Sensor found!");

}

void loop() {

    // motor direction
  digitalWrite(motorDirection1, HIGH);
  digitalWrite(motorDirection2, LOW);

  // motor speed
  analogWrite(motorSpeedPin, motorSpeed);

  unsigned long currentMillis = millis();

  uint8_t range = vl.readRange();
  uint8_t status = vl.readRangeStatus();

  if (range < 20 && !ledState) {
    Serial.print("Range: ");
    Serial.println(range);

    ledState = true;

    for (int i = triggerCount % 5; i < NUM_LEDS; i++) {
      leds3[i] = CRGB::White;
    }
    FastLED.show();
    // digitalWrite(ledPin, HIGH);
    previousMillis = currentMillis;

    triggerCount++;
    updateFirstStrip();
    
    yield();
  }

  if (ledState && currentMillis - previousMillis >= interval) {
    ledState = false;
    // digitalWrite(ledPin, LOW);
    for (int i = triggerCount % 5; i < NUM_LEDS; i++) {
      leds3[i] = CRGB::Black;
    }
    FastLED.show();
  }

  if (stripLit && millis() - stripPreviousMillis >= stripInterval) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }
    FastLED.show();
    triggerCount = 0;
    stripLit = false;
  }

  // Check if the second LED bar needs to be turned off
  if (strip2TriggerCount >= NUM_LEDS && millis() - strip2PreviousMillis >= strip2Interval) {
    resetSecondStrip();
    strip2TriggerCount = 0;  // Reset the trigger times of the second LED bar
  }

  if ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
    Serial.println("System error");
  }
}

void updateFirstStrip() {
  if (triggerCount % 5 == 0) {
    int ledIndex = ((triggerCount / 5) - 1);
    Serial.println(ledIndex);
    if (ledIndex < NUM_LEDS) {

      leds[ledIndex] = CRGB::White;

      FastLED.show();
      if (ledIndex == NUM_LEDS - 1) {
        stripPreviousMillis = millis();
        stripLit = true;
        updateSecondStrip();
      }
    }
  }
}

void updateSecondStrip() {
  int ledIndex2 = (strip2TriggerCount % NUM_LEDS) * 2;
  Serial.print("index2:");
  Serial.println(ledIndex2);

  leds2[ledIndex2] = CRGB::White;
  leds2[ledIndex2 + 1] = CRGB::White;
  FastLED.show();

  strip2TriggerCount++;
  Serial.print("trigger");
  Serial.println(strip2TriggerCount);
  if (strip2TriggerCount >= NUM_LEDS) {
    strip2PreviousMillis = millis();
  }
}

void resetSecondStrip() {
  for (int i = 0; i < NUM_LEDS * 2; i++) {
    leds2[i] = CRGB::Black;
  }
  FastLED.show();
  strip2TriggerCount = 0;
}