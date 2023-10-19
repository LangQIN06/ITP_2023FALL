// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

//RTC
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants
const int buttonPin = 2;                    // The digital pin where the button is connected
const int button1Pin = 3;
const int button2Pin = 4; 
const int servoPin = 9;                     // The digital pin where the servo is connected
unsigned long timerDuration = 0;  // 5 minutes in milliseconds

// Variables
Servo myservo;
int pos = 0;
bool timerStarted = false;
unsigned long startTime;

void setup() {
  myservo.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);  // Use the internal pull-up resistor
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(servoPin, OUTPUT);
  myservo.write(90); // Initialize servo position
  Serial.begin(57600);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("RTC Time demo!"));
  display.display();
  delay(2000);
}

void loop() {
   DateTime now = rtc.now();

    // Serial.print(now.year(), DEC);
    // Serial.print('/');
    // Serial.print(now.month(), DEC);
    // Serial.print('/');
    // Serial.print(now.day(), DEC);
    // Serial.print(" (");
    // Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // Serial.print(") ");
    // Serial.print(now.hour(), DEC);
    // Serial.print(':');
    // Serial.print(now.minute(), DEC);
    // Serial.print(':');
    // Serial.print(now.second(), DEC);
    // Serial.println();

    display.clearDisplay();
    display.setCursor(0,0);
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.println(now.second(), DEC);
    display.print(now.year(), DEC);
    display.print('/');
    display.print(now.month(), DEC);
    display.print('/');
    display.print(now.day(), DEC);
    display.print(" (");
    // display.print(daysOfTheWeek[now.dayOfTheWeek()]);
    // display.print(") ");
    display.display();


  if (digitalRead(buttonPin) == HIGH && !timerStarted) {
    timerDuration = 5000;
    timerStarted = true;
    startTime = millis();
    Serial.println("0 Timer started.");
  }

  if (digitalRead(button1Pin) == HIGH && !timerStarted) {
    timerDuration = 10000;
    timerStarted = true;
    startTime = millis();
    Serial.println("1 Timer started.");
  }

  if (digitalRead(button2Pin) == HIGH && !timerStarted) {
    timerDuration = 15000;
    timerStarted = true;
    startTime = millis();
    Serial.println("2 Timer started.");
  }

  if (timerStarted) {
    unsigned long currentTime = millis();
    if (currentTime - startTime >= timerDuration) {
      // Timer has elapsed
      timerStarted = false;
      // myservo.write(90); // Move the servo to 90 degrees

   for (int i = 0; i < 5; i++) { // Repeat the sequence 5 times
    for (int pos = 90; pos <= 120; pos += 2) {
      myservo.write(pos);
      delay(15);
    }
    for (int pos = 120; pos >= 90; pos -= 2) {
      myservo.write(pos);
      delay(15);
    }
  }
      Serial.println("Timer finished. Servo moved.");
    }
  }
}
