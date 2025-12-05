#include "idle.h"

IdleTask::IdleTask() : Task("IDLE") {
  id = IDLE_ID;
};

bool IdleTask::loop() {
  bool returnValue = false;
  lock.take();
  execution.start();
  returnValue = executeTask();
  execution.stop();
  lock.give();
  return returnValue;
}