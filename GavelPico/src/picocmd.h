#ifndef __GAVEL_PICO_H
#define __GAVEL_PICO_H

#include <GavelInterfaces.h>
#include <Terminal.h>

class PicoCommand {
public:
  Callback rebootCallBacks;
  void uploadPico(OutputInterface* terminal);
  void rebootPico(OutputInterface* terminal);
  std::function<void(TerminalLibrary::OutputInterface*)> uploadCmd();
  std::function<void(TerminalLibrary::OutputInterface*)> rebootCmd();

private:
};

#endif //__GAVEL_PICO_H
