#ifndef __GAVEL_BLINK_H
#define __GAVEL_BLINK_H

#include <GavelInterfaces.h>
#include <GavelTask.h>

class Blink : public Task, public Hardware {
public:
  Blink();
  virtual void addCmd(TerminalCommand* __termCmd) override {};
  virtual void reservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;
  virtual bool isWorking() const override { return true; };

private:
  bool state = true;
};

#endif //__GAVEL_BLINK_H
