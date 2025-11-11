#include "gpiomanager.h"

#include <Arduino.h>

static char taskname[] = "GPIOManager";

GPIOManager::GPIOManager() : Task(taskname){};

void GPIOManager::addCmd(TerminalCommand* __termCmd) {}

bool GPIOManager::setupTask(OutputInterface* __terminal) {
  return true;
}

bool GPIOManager::executeTask() {
  return true;
}

void GPIOManager::addPins(PinList* __pinList) {
  Pin* newPin;
  for (unsigned long i = 0; i < __pinList->count(); i++) {
    newPin = __pinList->get(i);
    pinList.push(newPin);
  }
}

int pinCmp(const void* a, const void* b) {
  Pin* pinA = (Pin*) a;
  Pin* pinB = (Pin*) b;
  int compareResult = 0;
  if (pinA->location == pinB->location)
    compareResult = 0;
  else if (pinA->location > pinB->location)
    compareResult = 1;
  else
    compareResult = -1;

  if (compareResult == 0) {
    if (pinA->pinNumber == pinB->pinNumber)
      compareResult = 0;
    else if (pinA->pinNumber > pinB->pinNumber)
      compareResult = 1;
    else
      compareResult = -1;
  }
  return (compareResult);
}