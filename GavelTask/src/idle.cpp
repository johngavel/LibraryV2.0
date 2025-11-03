#include "idle.h"

static char taskname[] = "IDLE";

IdleTask::IdleTask() : Task(taskname) {
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