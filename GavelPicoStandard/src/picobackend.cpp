
#include "picobackend.h"

#include <Arduino.h>
#include <GavelDebug.h>

#if defined ARDUINO_RASPBERRY_PI_PICO
static char devicename[] = "Pi Pico";
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
static char devicename[] = "Pi Pico W";
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
static char devicename[] = "RP2040 Zero";
#elif defined ARDUINO_GENERIC_RP2040
static char devicename[] = "Gavel Mini";
#else
#error "This architecture does not support this Hardware!"
#endif

RP2040Backend::RP2040Backend() : IGPIOBackend(devicename, GPIO_DEVICE_CPU_BOARD) {}

void RP2040Backend::setAvailablePins(BackendPinSetup* pinsetup) {
#if defined ARDUINO_RASPBERRY_PI_PICO
  pinsetup->addAvailablePin(getDeviceIndex(), 0);
  pinsetup->addAvailablePin(getDeviceIndex(), 1);
  pinsetup->addAvailablePin(getDeviceIndex(), 2);
  pinsetup->addAvailablePin(getDeviceIndex(), 3);
  pinsetup->addAvailablePin(getDeviceIndex(), 4);
  pinsetup->addAvailablePin(getDeviceIndex(), 5);
  pinsetup->addAvailablePin(getDeviceIndex(), 6);
  pinsetup->addAvailablePin(getDeviceIndex(), 7);
  pinsetup->addAvailablePin(getDeviceIndex(), 8);
  pinsetup->addAvailablePin(getDeviceIndex(), 9);
  pinsetup->addAvailablePin(getDeviceIndex(), 10);
  pinsetup->addAvailablePin(getDeviceIndex(), 11);
  pinsetup->addAvailablePin(getDeviceIndex(), 12);
  pinsetup->addAvailablePin(getDeviceIndex(), 13);
  pinsetup->addAvailablePin(getDeviceIndex(), 14);
  pinsetup->addAvailablePin(getDeviceIndex(), 15);
  pinsetup->addAvailablePin(getDeviceIndex(), 16);
  pinsetup->addAvailablePin(getDeviceIndex(), 17);
  pinsetup->addAvailablePin(getDeviceIndex(), 18);
  pinsetup->addAvailablePin(getDeviceIndex(), 19);
  pinsetup->addAvailablePin(getDeviceIndex(), 20);
  pinsetup->addAvailablePin(getDeviceIndex(), 21);
  pinsetup->addAvailablePin(getDeviceIndex(), 22);
  pinsetup->addAvailablePin(getDeviceIndex(), 25);
  pinsetup->addAvailablePin(getDeviceIndex(), 26);
  pinsetup->addAvailablePin(getDeviceIndex(), 27);
  pinsetup->addAvailablePin(getDeviceIndex(), 28);
  pinsetup->addAvailablePin(getDeviceIndex(), LED_BUILTIN);
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
  pinsetup->addAvailablePin(getDeviceIndex(), 0);
  pinsetup->addAvailablePin(getDeviceIndex(), 1);
  pinsetup->addAvailablePin(getDeviceIndex(), 2);
  pinsetup->addAvailablePin(getDeviceIndex(), 3);
  pinsetup->addAvailablePin(getDeviceIndex(), 4);
  pinsetup->addAvailablePin(getDeviceIndex(), 5);
  pinsetup->addAvailablePin(getDeviceIndex(), 6);
  pinsetup->addAvailablePin(getDeviceIndex(), 7);
  pinsetup->addAvailablePin(getDeviceIndex(), 8);
  pinsetup->addAvailablePin(getDeviceIndex(), 9);
  pinsetup->addAvailablePin(getDeviceIndex(), 10);
  pinsetup->addAvailablePin(getDeviceIndex(), 11);
  pinsetup->addAvailablePin(getDeviceIndex(), 12);
  pinsetup->addAvailablePin(getDeviceIndex(), 13);
  pinsetup->addAvailablePin(getDeviceIndex(), 14);
  pinsetup->addAvailablePin(getDeviceIndex(), 15);
  pinsetup->addAvailablePin(getDeviceIndex(), 16);
  pinsetup->addAvailablePin(getDeviceIndex(), 17);
  pinsetup->addAvailablePin(getDeviceIndex(), 18);
  pinsetup->addAvailablePin(getDeviceIndex(), 19);
  pinsetup->addAvailablePin(getDeviceIndex(), 20);
  pinsetup->addAvailablePin(getDeviceIndex(), 21);
  pinsetup->addAvailablePin(getDeviceIndex(), 22);
  pinsetup->addAvailablePin(getDeviceIndex(), 25);
  pinsetup->addAvailablePin(getDeviceIndex(), 26);
  pinsetup->addAvailablePin(getDeviceIndex(), 27);
  pinsetup->addAvailablePin(getDeviceIndex(), 28);
  pinsetup->addAvailablePin(getDeviceIndex(), LED_BUILTIN);
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
  pinsetup->addAvailablePin(getDeviceIndex(), 0);
  pinsetup->addAvailablePin(getDeviceIndex(), 1);
  pinsetup->addAvailablePin(getDeviceIndex(), 2);
  pinsetup->addAvailablePin(getDeviceIndex(), 3);
  pinsetup->addAvailablePin(getDeviceIndex(), 4);
  pinsetup->addAvailablePin(getDeviceIndex(), 5);
  pinsetup->addAvailablePin(getDeviceIndex(), 6);
  pinsetup->addAvailablePin(getDeviceIndex(), 7);
  pinsetup->addAvailablePin(getDeviceIndex(), 8);
  pinsetup->addAvailablePin(getDeviceIndex(), 9);
  pinsetup->addAvailablePin(getDeviceIndex(), 10);
  pinsetup->addAvailablePin(getDeviceIndex(), 11);
  pinsetup->addAvailablePin(getDeviceIndex(), 12);
  pinsetup->addAvailablePin(getDeviceIndex(), 13);
  pinsetup->addAvailablePin(getDeviceIndex(), 14);
  pinsetup->addAvailablePin(getDeviceIndex(), 15);
  pinsetup->addAvailablePin(getDeviceIndex(), 16);
  pinsetup->addAvailablePin(getDeviceIndex(), 26);
  pinsetup->addAvailablePin(getDeviceIndex(), 27);
  pinsetup->addAvailablePin(getDeviceIndex(), 28);
  pinsetup->addAvailablePin(getDeviceIndex(), 29);
#elif defined ARDUINO_GENERIC_RP2040
  pinsetup->addAvailablePin(getDeviceIndex(), 0);
  pinsetup->addAvailablePin(getDeviceIndex(), 1);
  pinsetup->addAvailablePin(getDeviceIndex(), 4);
  pinsetup->addAvailablePin(getDeviceIndex(), 5);
  pinsetup->addAvailablePin(getDeviceIndex(), 14);
  pinsetup->addAvailablePin(getDeviceIndex(), 15);
  pinsetup->addAvailablePin(getDeviceIndex(), 16);
  pinsetup->addAvailablePin(getDeviceIndex(), 17);
  pinsetup->addAvailablePin(getDeviceIndex(), 18);
  pinsetup->addAvailablePin(getDeviceIndex(), 19);
  pinsetup->addAvailablePin(getDeviceIndex(), LED_BUILTIN);
  pinsetup->addAvailablePin(getDeviceIndex(), 26);
  pinsetup->addAvailablePin(getDeviceIndex(), 27);
  pinsetup->addAvailablePin(getDeviceIndex(), 28);
#else
#error "This architecture does not support this Hardware!"
#endif
  return;
};

void RP2040Backend::setReservePins(BackendPinSetup* pinsetup) {}

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
  bool read = digitalRead(pin);
  return read;
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
