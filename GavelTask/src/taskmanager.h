#ifndef __GAVEL_TASK_MANAGER_H
#define __GAVEL_TASK_MANAGER_H

#include "idle.h"
#include "taskclass.h"

#include <GavelUtil.h>

#define CPU_CORES 2

class TaskManager : public Task {
public:
  TaskManager();
  void add(Task* task);
  Task* getTask(unsigned long i);
  unsigned long getTaskCount();
  void addCmd(TerminalCommand* __termCmd) override;
  void reservePins(BackendPinSetup* pinsetup) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool loop();
  bool executeTask() override;
  void system(OutputInterface* terminal);

private:
  ClassicQueue queue;
  void setupIdle();
  IdleTask idleTask[CPU_CORES];
};

#endif // __GAVEL_TASK_MANAGER_H