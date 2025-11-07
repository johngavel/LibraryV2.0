#include "taskclass.h"

#include <GavelUtil.h>

bool Task::setup(OutputInterface* __terminal) {
  bool returnValue = false;
  terminal = __terminal;
  lock.take();
  returnValue = setupTask(__terminal);
  lock.give();
  return returnValue;
};

bool Task::loop() {
  bool returnValue = false;
  lock.take();
  if (expired()) {
    execution.start();
    returnValue = executeTask();
    execution.stop();
  }
  if (getTimerRun() == false) {
    execution.start();
    execution.stop();
  }
  lock.give();
  return returnValue;
};
