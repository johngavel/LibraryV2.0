#include "timer.h"

static const unsigned long MAX_TIME_REMAINING = 1000000;
static const int MAX_RETURN_COUNT = 1000;

bool Timer::expired() {
  return (expiredMicro(micros()) > 0);
}

int Timer::expiredMilli(unsigned long timeStamp) {
  return expiredMicro(timeStamp * 1000);
}

int Timer::expiredMicro(unsigned long timeStamp) {
  int returnCount = 0;

  if (timeout == 0) {
    returnCount = 1; // Always runs when timeout == 0
  } else {
    while (run && ((timeStamp - refresh) >= timeout)) {
      refresh += timeout;
      returnCount++;
      if (returnCount > MAX_RETURN_COUNT) { reset(timeStamp); }
    }
  }
  return returnCount;
}

unsigned long Timer::timeRemainingMicro() {
  unsigned long timeStamp = micros();
  unsigned long timeRemaining = MAX_TIME_REMAINING;
  if (run) {
    unsigned long tempTime = timeout - (timeStamp - refresh);
    if (tempTime < timeRemaining) timeRemaining = tempTime;
  }
  return timeRemaining;
}
