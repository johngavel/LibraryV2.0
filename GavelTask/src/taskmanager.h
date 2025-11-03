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
  void addCmd(TerminalCommand* __termCmd);
  bool setupTask(OutputInterface* __terminal);
  bool loop();
  bool executeTask();
  void system(OutputInterface* terminal);
  std::function<void(TerminalLibrary::OutputInterface*)> getSystemCallback();

private:
  ClassicQueue queue;
  void setupIdle();
  IdleTask idleTask[CPU_CORES];
};

#endif