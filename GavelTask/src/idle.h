#ifndef __GAVEL_IDLE_H
#define __GAVEL_IDLE_H

#include "taskclass.h"

#define MAX_IDLE_TIME 100
#define MIN_IDLE_TIME 2

class IdleTask : public Task {
public:
  IdleTask();
  virtual void addCmd(TerminalCommand* __termCmd) override {};
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override {
    setRefreshMilli(MAX_IDLE_TIME);
    return true;
  };
  bool loop();
  virtual bool executeTask() override {
    delay(delay_ms);
    delay_ms = 0;
    return true;
  };
  void setDelay(unsigned long __delay_ms) {
    unsigned long __time = (__delay_ms > MAX_IDLE_TIME) ? MAX_IDLE_TIME : __delay_ms;
    __time = (__time > MIN_IDLE_TIME) ? __time : 0;
    lock.take();
    delay_ms = __time;
    lock.give();
  };

private:
  unsigned int delay_ms = 0;
};

#endif // __GAVEL_IDLE_H