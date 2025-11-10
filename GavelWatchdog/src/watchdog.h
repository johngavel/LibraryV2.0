#ifndef __GAVEL_WATCHDOG
#define __GAVEL_WATCHDOG

#include <GavelTask.h>
#include <Terminal.h>

class Watchdog : public Task {
public:
  Watchdog(uint32_t timeout = 8300, uint32_t petCycle = 1000);
  void addCmd(TerminalCommand* __termCmd) override {};
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  void monitor(unsigned long core, bool monitor);

private:
  uint32_t watchdogTimeout;
  uint32_t watchdogPetCycle;
  bool monitorCore[CPU_CORES];
  OutputInterface* terminal;
};

#endif