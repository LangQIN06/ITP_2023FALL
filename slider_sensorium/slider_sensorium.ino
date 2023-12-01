/*
 ____  _____ _        _
| __ )| ____| |      / \
|  _ \|  _| | |     / _ \
| |_) | |___| |___ / ___ \
|____/|_____|_____/_/   \_\
http://bela.io

This example allows you to create a slider from an arbitrary number of pads
which can be in any order. This example uses a Trill Flex sensor with a
custom flexible sensor with 3 sliders next to each other. See
learn.bela.io/flex-tutorial/ for more info on the sensor.

Trill Flex has 30 capactive pads in total and we are splitting this
into 3 groups of 10 pads. The order of the pads and their pin numbering is
defined in slider0Pads etc. We can also set the max number of centroid
which will define how many touches can be registered per slider. This
is currently set to 3 meaning that 3 individual touch points can be registered
per sensor.

Each touch has a location and a touch size which equates to how hard the finger
is pushing on the sensor. This example is particularly useful for working with
Trill Flex and Trill Craft. When working with these sensors it always important
to check that the Prescaler and Noisethreshold settings are optimum for your
application. Experiment with different values if you are not getting a reading
or seeing lots of cross talk between the sensors.
*/

#include <Trill.h>
#include <Keypad.h>

Trill trillSensor;

const unsigned int NUM_TOTAL_PADS = 30;
CustomSlider::WORD rawData[NUM_TOTAL_PADS];

const uint8_t slider0NumPads = 10;
const uint8_t slider1NumPads = 10;
const uint8_t slider2NumPads = 10;
const int ledPins[3] = { 3, 5, 6 };
// const int resetButtonPin = 7;

const byte ROWS = 1;   //four rows
const byte COLS = 1;   //three columns
const byte ROWSM = 1;  //rows
const byte RESET = 1;  //rows
char keyF[ROWS][COLS] = {
  { '1' }
};
char keyM[ROWSM][COLS] = {
  { '2' }
};
char keyR[RESET][COLS] = {
  { '3' }
};

byte rowPins[ROWS] = { 11 };    //connect to the row pinouts of the keypad
byte rowPinsM[ROWSM] = { 12 };  //connect to the row pinouts of the keypad
byte rowPinsR[RESET] = { 7 };   //connect to the row pinouts of the keypad
byte colPins[COLS] = { 13 };    //connect to the column pinouts of the keypad

Keypad keypadF = Keypad(makeKeymap(keyF), rowPinsM, colPins, ROWS, COLS);
Keypad keypadM = Keypad(makeKeymap(keyM), rowPins, colPins, ROWSM, COLS);
Keypad keypadR = Keypad(makeKeymap(keyR), rowPinsR, colPins, RESET, COLS);

bool resetPressed = true;
bool sliderActive = false;  // if slider has been touched
int activeSlider = -1;      // index of current actively slider

// Order of the pads used by each slider
uint8_t slider0Pads[slider0NumPads] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
uint8_t slider1Pads[slider1NumPads] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
uint8_t slider2Pads[slider2NumPads] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };

const unsigned int maxNumCentroids = 3;
const unsigned int numSliders = 3;
CustomSlider sliders[numSliders];

void setup() {
  sliders[0].setup(slider0Pads, slider0NumPads);
  sliders[1].setup(slider1Pads, slider1NumPads);
  sliders[2].setup(slider2Pads, slider2NumPads);
  // Initialise serial and touch sensor
  Serial.begin(115200);
  int ret;
  while (trillSensor.setup(Trill::TRILL_FLEX)) {
    //Serial.println("failed to initialise trillSensor");
    //Serial.println("Retrying...");
    delay(100);
  }
  //Serial.println("Success initialising trillSensor");
  trillSensor.setMode(Trill::DIFF);
  // We recommend a prescaler value of 4
  trillSensor.setPrescaler(4);
  // Experiment with this value to avoid corss talk between sliders if they are position close together
  trillSensor.setNoiseThreshold(200);

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {

  //gender expression:
  char keyF = keypadF.getKey();
  char keyM = keypadM.getKey();
  char keyR = keypadR.getKey();

  if (keyF) {
    digitalWrite(9, HIGH);
    digitalWrite(10, LOW);
  }

  if (keyM) {
    digitalWrite(10, HIGH);
    digitalWrite(9, LOW);
  }


  if (keyR) {
    sliderActive = false;  // allow touching again
    activeSlider = -1;

    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPins[i], LOW);  // turn off LED
    }
    delay(200);
  }



  // Read 20 times per second
  delay(50);
  if (!trillSensor.requestRawData()) {
    Serial.println("Failed reading from device. Is it disconnected?");
    return setup();
  }
  unsigned n = 0;
  // read all the data from the device into a local buffer
  while (trillSensor.rawDataAvailable() > 0 && n < NUM_TOTAL_PADS) {
    rawData[n++] = trillSensor.rawDataRead();
  }


  for (uint8_t n = 0; n < numSliders; ++n) {
    sliders[n].process(rawData);
    if (sliders[n].getNumTouches() > 0) {

      // Serial.print("[");
      // Serial.print(sliders[n].getNumTouches());
      // Serial.print("]: ");
      if (sliders[n].getNumTouches() > 0) {

        if (!sliderActive || activeSlider == n) {
          Serial.print("button");
          Serial.print(n);
          Serial.print(" ");
          for (int i = 0; i < sliders[n].getNumTouches(); i++) {
            Serial.print(sliders[n].touchLocation(i));
            Serial.print(" ");
          }
          // get location to brightness
          float touchLocation = sliders[n].touchLocation(0);
          int brightness = map(touchLocation, 0, 1152, 0, 255);
          analogWrite(ledPins[n], brightness);

          sliderActive = true;
          activeSlider = n;
        }
      }
    }
  }
  Serial.println("");
}
