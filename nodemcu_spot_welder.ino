#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include <Fonts/FreeSans9pt7b.h>

#define ROTARY_ENCODER_A_PIN 2
#define ROTARY_ENCODER_B_PIN 3
#define ROTARY_ENCODER_STEPS 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define TRIGGER_BUTTON_PIN 4
#define RELAY_PIN 5


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Encoder rotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);

long prevMilliseconds = 0;
long relayMilliseconds = 0;
long loopCount = 0;
bool prevRelayOn = false;
bool relayOn = false;
int buttonCurrent = HIGH;
int buttonPrev = HIGH;

void printEncoderReading() {
  display.clearDisplay();
  display.setCursor(0, 15);
  
  display.println(".V.Burninator.V.");
  display.println((String)relayMilliseconds + " ms");
  
  if (relayOn) {
    display.println("Relay: BURN");
  } else {
    display.println("Relay: OFF");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  bool circleValues = false;
  pinMode(TRIGGER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  printEncoderReading();
}

void checkButton() {
  int buttonCurrent = digitalRead(TRIGGER_BUTTON_PIN);
  prevRelayOn = relayOn;

  if (buttonCurrent == LOW && buttonPrev == HIGH) {
    relayOn = true;
  }

  buttonPrev = buttonCurrent;
}

void triggerRelay() {
  if (relayOn) {
    // Flip relay pin
    Serial.println("Relay on");
    printEncoderReading();
    
    digitalWrite(RELAY_PIN, HIGH);
    delay(relayMilliseconds);
    digitalWrite(RELAY_PIN, LOW);

    relayOn = false;
    
    Serial.println("Relay off");
    printEncoderReading();
  }
}

void loop() {
  // rotary_loop();
  long reading = rotaryEncoder.read();

  if (reading < 0){
    relayMilliseconds = 0;
    rotaryEncoder.write(0);
  } else {
    relayMilliseconds = (reading / 4) * 10;
  }

  loopCount = loopCount + 1;

  checkButton();

  if (prevMilliseconds != relayMilliseconds) {
    printEncoderReading();
  }
  prevMilliseconds = relayMilliseconds;

  triggerRelay();
}