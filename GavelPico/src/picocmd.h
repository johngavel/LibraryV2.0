#ifndef __GAVEL_PICO_H
#define __GAVEL_PICO_H

#include <GavelInterfaces.h>
#include <Terminal.h>

class PicoCommand {
public:
  Callback rebootCallBacks;
  void uploadPico(OutputInterface* terminal = nullptr);
  void rebootPico(OutputInterface* terminal = nullptr);

private:
};

#endif //__GAVEL_PICO_H
