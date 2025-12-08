#include "onboardled.h"

#include <Arduino.h>

Blink::Blink() : Task("Blink"){};

void Blink::reservePins(BackendPinSetup* pinsetup) {
  pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, LED_BUILTIN, "Internal LED");
}

bool Blink::setupTask(OutputInterface* __terminal) {
  setRefreshMilli(500);
  pinMode(LED_BUILTIN, OUTPUT);
  return true;
}

bool Blink::executeTask() {
  state = !state;
  digitalWrite(LED_BUILTIN, state);
  return true;
}
