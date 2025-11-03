#ifndef __GAVEL_BLINK_H
#define __GAVEL_BLINK_H

#include <GavelTask.h>

class Blink : public Task {
public:
  Blink();
  virtual void addCmd(TerminalCommand* __termCmd) {};
  virtual bool setupTask(OutputInterface* __terminal);
  virtual bool executeTask();

private:
  static Blink* blink;
  bool state = true;
};

#endif
