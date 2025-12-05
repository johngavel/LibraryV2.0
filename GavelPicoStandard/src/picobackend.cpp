
#include "picobackend.h"

#include <Arduino.h>

#if defined ARDUINO_RASPBERRY_PI_PICO
static char devicename[] = "Raspberry Pi Pico";
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
static char devicename[] = "Raspberry Pi Pico W";
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
static char devicename[] = "RP2040 Zero";
#elif defined ARDUINO_GENERIC_RP2040
static char devicename[] = "Gavel Mini Pico";
#else
#error "This architecture does not support this Hardware!"
#endif

RP2040Backend::RP2040Backend() : IGPIOBackend(devicename) {}

static PinList availablePins;
PinList* RP2040Backend::getAvailablePins() {
  availablePins.numberOfPins = 0;
#if defined ARDUINO_RASPBERRY_PI_PICO
  availablePins.pin[availablePins.numberOfPins++] = 0;
  availablePins.pin[availablePins.numberOfPins++] = 1;
  availablePins.pin[availablePins.numberOfPins++] = 2;
  availablePins.pin[availablePins.numberOfPins++] = 3;
  availablePins.pin[availablePins.numberOfPins++] = 4;
  availablePins.pin[availablePins.numberOfPins++] = 5;
  availablePins.pin[availablePins.numberOfPins++] = 6;
  availablePins.pin[availablePins.numberOfPins++] = 7;
  availablePins.pin[availablePins.numberOfPins++] = 8;
  availablePins.pin[availablePins.numberOfPins++] = 9;
  availablePins.pin[availablePins.numberOfPins++] = 10;
  availablePins.pin[availablePins.numberOfPins++] = 11;
  availablePins.pin[availablePins.numberOfPins++] = 12;
  availablePins.pin[availablePins.numberOfPins++] = 13;
  availablePins.pin[availablePins.numberOfPins++] = 14;
  availablePins.pin[availablePins.numberOfPins++] = 15;
  availablePins.pin[availablePins.numberOfPins++] = 16;
  availablePins.pin[availablePins.numberOfPins++] = 17;
  availablePins.pin[availablePins.numberOfPins++] = 18;
  availablePins.pin[availablePins.numberOfPins++] = 19;
  availablePins.pin[availablePins.numberOfPins++] = 20;
  availablePins.pin[availablePins.numberOfPins++] = 21;
  availablePins.pin[availablePins.numberOfPins++] = 22;
  availablePins.pin[availablePins.numberOfPins++] = 25;
  availablePins.pin[availablePins.numberOfPins++] = 26;
  availablePins.pin[availablePins.numberOfPins++] = 27;
  availablePins.pin[availablePins.numberOfPins++] = 28;
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
  availablePins.pin[availablePins.numberOfPins++] = 0;
  availablePins.pin[availablePins.numberOfPins++] = 1;
  availablePins.pin[availablePins.numberOfPins++] = 2;
  availablePins.pin[availablePins.numberOfPins++] = 3;
  availablePins.pin[availablePins.numberOfPins++] = 4;
  availablePins.pin[availablePins.numberOfPins++] = 5;
  availablePins.pin[availablePins.numberOfPins++] = 6;
  availablePins.pin[availablePins.numberOfPins++] = 7;
  availablePins.pin[availablePins.numberOfPins++] = 8;
  availablePins.pin[availablePins.numberOfPins++] = 9;
  availablePins.pin[availablePins.numberOfPins++] = 10;
  availablePins.pin[availablePins.numberOfPins++] = 11;
  availablePins.pin[availablePins.numberOfPins++] = 12;
  availablePins.pin[availablePins.numberOfPins++] = 13;
  availablePins.pin[availablePins.numberOfPins++] = 14;
  availablePins.pin[availablePins.numberOfPins++] = 15;
  availablePins.pin[availablePins.numberOfPins++] = 16;
  availablePins.pin[availablePins.numberOfPins++] = 17;
  availablePins.pin[availablePins.numberOfPins++] = 18;
  availablePins.pin[availablePins.numberOfPins++] = 19;
  availablePins.pin[availablePins.numberOfPins++] = 20;
  availablePins.pin[availablePins.numberOfPins++] = 21;
  availablePins.pin[availablePins.numberOfPins++] = 22;
  availablePins.pin[availablePins.numberOfPins++] = 25;
  availablePins.pin[availablePins.numberOfPins++] = 26;
  availablePins.pin[availablePins.numberOfPins++] = 27;
  availablePins.pin[availablePins.numberOfPins++] = 28;
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
  availablePins.pin[availablePins.numberOfPins++] = 0;
  availablePins.pin[availablePins.numberOfPins++] = 1;
  availablePins.pin[availablePins.numberOfPins++] = 2;
  availablePins.pin[availablePins.numberOfPins++] = 3;
  availablePins.pin[availablePins.numberOfPins++] = 4;
  availablePins.pin[availablePins.numberOfPins++] = 5;
  availablePins.pin[availablePins.numberOfPins++] = 6;
  availablePins.pin[availablePins.numberOfPins++] = 7;
  availablePins.pin[availablePins.numberOfPins++] = 8;
  availablePins.pin[availablePins.numberOfPins++] = 9;
  availablePins.pin[availablePins.numberOfPins++] = 10;
  availablePins.pin[availablePins.numberOfPins++] = 11;
  availablePins.pin[availablePins.numberOfPins++] = 12;
  availablePins.pin[availablePins.numberOfPins++] = 13;
  availablePins.pin[availablePins.numberOfPins++] = 14;
  availablePins.pin[availablePins.numberOfPins++] = 15;
  availablePins.pin[availablePins.numberOfPins++] = 16;
  availablePins.pin[availablePins.numberOfPins++] = 26;
  availablePins.pin[availablePins.numberOfPins++] = 27;
  availablePins.pin[availablePins.numberOfPins++] = 28;
  availablePins.pin[availablePins.numberOfPins++] = 29;
#elif defined ARDUINO_GENERIC_RP2040
  availablePins.pin[availablePins.numberOfPins++] = 0;
  availablePins.pin[availablePins.numberOfPins++] = 1;
  availablePins.pin[availablePins.numberOfPins++] = 4;
  availablePins.pin[availablePins.numberOfPins++] = 5;
  availablePins.pin[availablePins.numberOfPins++] = 14;
  availablePins.pin[availablePins.numberOfPins++] = 15;
  availablePins.pin[availablePins.numberOfPins++] = 16;
  availablePins.pin[availablePins.numberOfPins++] = 17;
  availablePins.pin[availablePins.numberOfPins++] = 18;
  availablePins.pin[availablePins.numberOfPins++] = 19;
  availablePins.pin[availablePins.numberOfPins++] = 25;
#else
#error "This architecture does not support this Hardware!"
#endif
  return &availablePins;
};

bool RP2040Backend::setupInput(int pin) {
  pinMode(pin, INPUT);
  return true;
}
bool RP2040Backend::setupOutput(int pin) {
  pinMode(pin, OUTPUT);
  return true;
}
bool RP2040Backend::setupAdc(int pin, unsigned char bits) {
  analogReadResolution(bits);
  pinMode(pin, INPUT);
  return true;
}

bool RP2040Backend::readDigital(int pin) const {
  return digitalRead(pin);
}
void RP2040Backend::writeDigital(int pin, bool level) {
  digitalWrite(pin, level);
}
unsigned int RP2040Backend::readAnalog(int pin) const {
  return analogRead(pin);
}

void RP2040Backend::pwmConfigure(int pin, unsigned long freqHz, unsigned int dutyPct) {
  analogWriteFreq(freqHz);
  analogWriteRange(100);
  analogWrite(pin, dutyPct);
}
void RP2040Backend::toneStart(int pin, unsigned long freqHz) {
  tone(pin, freqHz);
}
void RP2040Backend::toneStop(int pin) {
  noTone(pin);
}
