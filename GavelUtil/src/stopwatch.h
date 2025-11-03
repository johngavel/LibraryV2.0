#ifndef __GAVEL_STOP_WATCH
#define __GAVEL_STOP_WATCH

#include "average.h"

#include <Arduino.h>

class StopWatch {
public:
  StopWatch() : startTime(0), finishTime(0){};
  void start(unsigned long __time = micros()) { startTime = __time; };
  void stop(unsigned long __time = micros()) { finishTime = __time; };
  unsigned long time() { return finishTime - startTime; };

private:
  unsigned long startTime;
  unsigned long finishTime;
};

class AvgStopWatch : public StopWatch {
public:
  AvgStopWatch(unsigned long factor = 20) { average.setWindowSize(factor); };
  void stop() {
    StopWatch::stop();
    lwm = min(StopWatch::time(), lwm);
    hwm = max(StopWatch::time(), hwm);
    average.sample(StopWatch::time());
  };
  unsigned long time() { return average.getAverage(); };
  unsigned long lowWaterMark() {
    unsigned long i = lwm;
    lwm = WINT_MAX;
    return i;
  };
  unsigned long highWaterMark() {
    unsigned long i = hwm;
    hwm = 0;
    return i;
  };

private:
  Average average;
  unsigned long lwm = WINT_MAX;
  unsigned long hwm = 0;
};

#endif