#ifndef __GAVEL_BLINK_H
#define __GAVEL_BLINK_H

#include <GavelTask.h>

class Blink : public Task {
public:
  Blink();
  void addCmd(TerminalCommand* __termCmd) override {};
  void reservePins(BackendPinSetup* pinsetup) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

private:
  bool state = true;
};

#endif //__GAVEL_BLINK_H
