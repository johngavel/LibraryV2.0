#ifndef __GAVEL_TASK_H
#define __GAVEL_TASK_H

#include <GavelInterfaces.h>
#include <GavelUtil.h>
#include <Terminal.h>

#define TASK_NAME_LENGTH 20
#define TASK_MANAGER_ID 3000
#define IDLE_ID 4000

class Task : public Timer, public Identifiable {
public:
  Task(const char* __name) { strncpy(name, __name, TASK_NAME_LENGTH); };
  bool setup(OutputInterface* __terminal);
  bool loop();
  virtual void addCmd(TerminalCommand* __termCmd) = 0;
  virtual void reservePins(BackendPinSetup* pinsetup) = 0;
  virtual bool setupTask(OutputInterface* __terminal) = 0;
  virtual bool executeTask() = 0;
  unsigned long getRefreshRate() { return getRefreshMicro(); };
  int getCore() { return core; };
  void setCore(int __core) { core = __core; };
  char* getName() { return name; };
  void setName(char* __name) { strncpy(name, __name, TASK_NAME_LENGTH); };
  unsigned int getID() { return id; };
  AvgStopWatch* getExecutionTime() { return &execution; };
  bool runTask(bool __run) {
    run = __run;
    return run;
  };
  bool runTask() { return run; };

protected:
  SemLock lock;
  OutputInterface* terminal = nullptr;
  AvgStopWatch execution;

private:
  char name[TASK_NAME_LENGTH];
  int core = 0;
  bool run = true;
};

#endif // __GAVEL_TASK_H