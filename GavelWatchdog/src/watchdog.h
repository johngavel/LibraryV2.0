#ifndef __GAVEL_WATCHDOG_CLASS_H
#define __GAVEL_WATCHDOG_CLASS_H

#include <GavelTask.h>
#include <Terminal.h>

class Watchdog : public Task {
public:
  Watchdog(unsigned long timeout = 8300, unsigned long petCycle = 1000);
  void addCmd(TerminalCommand* __termCmd) override {};
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  void monitor(unsigned long core, bool monitor);

private:
  unsigned long watchdogTimeout;
  unsigned long watchdogPetCycle;
  bool monitorCore[CPU_CORES];
  OutputInterface* terminal;
};

#endif //__GAVEL_WATCHDOG_CLASS_H