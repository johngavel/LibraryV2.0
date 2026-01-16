#ifndef __GAVEL_SCANNER_H
#define __GAVEL_SCANNER_H

#include <GavelTask.h>

class Scan : public Task {
public:
  Scan();
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupTask(OutputInterface* __terminal) override { return true; };
  virtual bool executeTask() override { return true; };

  void scani2c(OutputInterface* terminal);

private:
};

#endif // __GAVEL_SCANNER_H
