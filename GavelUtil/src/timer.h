#ifndef __GAVEL_TIMER
#define __GAVEL_TIMER

#include <Arduino.h>

class Timer {
public:
  Timer() : refresh(micros()), timeout(100000), run(true){};
  void setRefreshSeconds(unsigned long time) { timeout = (unsigned long) (time * 1000000); };
  void setRefreshMilli(unsigned long time) { timeout = (unsigned long) (time * 1000); };
  void setRefreshMicro(unsigned long time) { timeout = time; };
  unsigned long getRefreshSeconds() { return (timeout / 1000000); };
  unsigned long getRefreshMilli() { return (timeout / 1000); };
  unsigned long getRefreshMicro() { return (timeout); };
  void runTimer(bool __run, unsigned long __refresh = micros()) {
    run = __run;
    reset(__refresh);
  };
  bool getTimerRun() { return run; };
  bool expired();
  unsigned long timeRemainingSecond() { return (timeRemainingMilli() / 1000); };
  unsigned long timeRemainingMilli() { return (timeRemainingMicro() / 1000); };
  unsigned long timeRemainingMicro();
  int expiredMilli(unsigned long timeStamp);
  int expiredMicro(unsigned long timeStamp);
  void reset(unsigned long __refresh = micros()) { refresh = __refresh; };

  unsigned long getLastExpired() { return refresh; };

private:
  unsigned long refresh;
  unsigned long timeout;
  bool run;
};

#endif
