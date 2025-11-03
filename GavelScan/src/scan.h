#ifndef __GAVEL_SCANNER
#define __GAVEL_SCANNER

#include <GavelTask.h>

class Scan : public Task {
public:
  Scan(){};
  virtual void addCmd(TerminalCommand* __termCmd);
  virtual bool setupTask(OutputInterface* __terminal);
  virtual bool executeTask() {};

  void scani2c(OutputInterface* terminal);
  std::function<void(TerminalLibrary::OutputInterface*)> scanCmd();

private:
};

#endif
