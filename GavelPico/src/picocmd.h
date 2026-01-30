#ifndef __GAVEL_PICO_CMD_H
#define __GAVEL_PICO_CMD_H

#include <GavelInterfaces.h>
#include <Terminal.h>

class PicoCommand : public DeviceCmd {
public:
  Callback rebootCallBacks;
  virtual void upload(OutputInterface* terminal = nullptr) override;
  virtual void reboot(OutputInterface* terminal = nullptr) override;
  virtual void upgrade(OutputInterface* terminal = nullptr) override;

private:
};

#endif //__GAVEL_PICO_CMD_H
