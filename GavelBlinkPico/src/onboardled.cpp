#include "onboardled.h"

#include <Arduino.h>

static char taskname[] = "Blink";

Blink::Blink() : Task(taskname){};

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
