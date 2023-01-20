#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 12
#define ROTARY_ENCODER_BUTTON_PIN 13
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define RELAY_PIN 10
#define TRIGGER_BUTTON_PIN 6

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

long relayMilliseconds = 0;
long loopCount = 0;
bool relayOn = false;

void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  bool circleValues = false;
  rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
  rotaryEncoder.setBoundaries(0, 500, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  // rotaryEncoder.disableAcceleration();
}

void printEncoderReading() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println((String)relayMilliseconds + " ms");
  // display.println("loop: " + (String)loopCount);
  display.display();
}

void loop() {
  // rotary_loop();
  relayMilliseconds = rotaryEncoder.readEncoder() * 10;
  loopCount = loopCount + 1;
  printEncoderReading();

  if (rotaryEncoder.isEncoderButtonClicked()) {
    // Flip relay pin
    Serial.println("Relay on");
    delay(relayMilliseconds);
    Serial.println("Relay off");
    // Flip relay pin    
  }
}