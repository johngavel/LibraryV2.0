#include "onboardled.h"

#include <Arduino.h>

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
#include <Adafruit_NeoPixel.h>
#define RP2040_PIN 16
#define NUMPIXELS 1
static Adafruit_NeoPixel pixels(NUMPIXELS, RP2040_PIN, NEO_GRB + NEO_KHZ800);
#endif

Blink::Blink() : Task("Blink"){};

void Blink::reservePins(BackendPinSetup* pinsetup) {
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  if (pinsetup != nullptr) { pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, RP2040_PIN, "Internal LED"); }
#else
  if (pinsetup != nullptr) { pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, LED_BUILTIN, "Internal LED"); }
#endif
}

bool Blink::setupTask(OutputInterface* __terminal) {
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  pixels.begin();
  setRefreshMilli(50);
#else
  setRefreshMilli(500);
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  return true;
}

bool Blink::executeTask() {
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  const long BLINK_TIME_MS = 2000;
  const long BRIGHTNESS = 20; // MAX BRIGHTNESS is 255
  const long BRIGHTNESS_PER_TIME = ((BLINK_TIME_MS / BRIGHTNESS) / 2);
  long brightness = 0;
  long blinkTime = 0;
  long color = 0;
  blinkTime = millis() % BLINK_TIME_MS;
  if (blinkTime < (BLINK_TIME_MS / 2))
    brightness = blinkTime / BRIGHTNESS_PER_TIME;
  else
    brightness = (BRIGHTNESS * 2) - (blinkTime / BRIGHTNESS_PER_TIME);
  brightness = constrain(brightness, 0, 255);
  color = (millis() / BLINK_TIME_MS) % 7;
  switch (color) {
  case 0: pixels.setPixelColor(0, pixels.Color(brightness, 0, 0)); break;
  case 1: pixels.setPixelColor(0, pixels.Color(0, brightness, 0)); break;
  case 2: pixels.setPixelColor(0, pixels.Color(0, 0, brightness)); break;
  case 3: pixels.setPixelColor(0, pixels.Color(brightness, brightness, 0)); break;
  case 4: pixels.setPixelColor(0, pixels.Color(brightness, 0, brightness)); break;
  case 5: pixels.setPixelColor(0, pixels.Color(0, brightness, brightness)); break;
  default: pixels.setPixelColor(0, pixels.Color(brightness, brightness, brightness)); break;
  }
  pixels.show();
#else
  state = !state;
  digitalWrite(LED_BUILTIN, state);
#endif
  return true;
}
