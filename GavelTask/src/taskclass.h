#ifndef __GAVEL_TASK_H
#define __GAVEL_TASK_H

#include <GavelInterfaces.h>
#include <GavelUtil.h>
#include <Terminal.h>

#define CPU_CORES 2

class Task : public Timer, public Identifiable {
public:
  Task() = delete;
  Task(const char* __name) : Identifiable(taskID(), __name){};
  Task(const char* __name, IdGenerator& generator) : Identifiable(generator, __name){};
  bool setup(OutputInterface* __terminal);
  bool loop();
  virtual void addCmd(TerminalCommand* __termCmd) = 0;
  virtual void reservePins(BackendPinSetup* pinsetup) = 0;
  virtual bool setupTask(OutputInterface* __terminal) = 0;
  virtual bool executeTask() = 0;
  unsigned long getRefreshRate() { return getRefreshMicro(); };
  int getCore() { return core; };
  void setCore(int __core) { core = __core; };
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
  int core = 0;
  bool run = true;
};

#endif // __GAVEL_TASK_H