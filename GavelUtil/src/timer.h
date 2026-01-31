#ifndef __GAVEL_TIMER_H
#define __GAVEL_TIMER_H

#include <Arduino.h>

class Timer {
public:
  Timer() : refresh(micros()), timeout(100000), run(true){};
  void setRefreshHours(unsigned long time) { setRefreshMinutes(60 * time); };
  void setRefreshMinutes(unsigned long time) { setRefreshSeconds(60 * time); };
  void setRefreshSeconds(unsigned long time) { setRefreshMilli(1000 * time); };
  void setRefreshMilli(unsigned long time) { setRefreshMicro(1000 * time); };
  void setRefreshMicro(unsigned long time) { timeout = time; };
  unsigned long getRefreshHours() { return (getRefreshMinutes() / 60); };
  unsigned long getRefreshMinutes() { return (getRefreshSeconds() / 60); };
  unsigned long getRefreshSeconds() { return (getRefreshMilli() / 1000); };
  unsigned long getRefreshMilli() { return (getRefreshMicro() / 1000); };
  unsigned long getRefreshMicro() { return (timeout); };
  void runTimer(bool __run, unsigned long __refresh = micros()) {
    run = __run;
    reset(__refresh);
  };
  bool getTimerRun() { return run; };
  bool expired();
  unsigned long timeRemainingHour() { return (timeRemainingMinute() / 60); };
  unsigned long timeRemainingMinute() { return (timeRemainingSecond() / 60); };
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

#endif //__GAVEL_TIMER_H
