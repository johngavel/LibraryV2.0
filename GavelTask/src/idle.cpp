#include "idle.h"

IdleTask::IdleTask() : Task("IDLE", idleID()){};

bool IdleTask::loop() {
  bool returnValue = false;
  lock.take();
  execution.start();
  returnValue = executeTask();
  execution.stop();
  lock.give();
  return returnValue;
}