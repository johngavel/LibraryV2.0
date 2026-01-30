#ifndef __GAVEL_DEVICE_CMD_H
#define __GAVEL_DEVICE_CMD_H

#include <functional>
#include <iostream>
#include <vector>

class DeviceCmd {
public:
  Callback rebootCallBacks;
  virtual void upload(OutputInterface* terminal = nullptr) = 0;
  virtual void reboot(OutputInterface* terminal = nullptr) = 0;
  virtual void upgrade(OutputInterface* terminal = nullptr) = 0;
};

#endif // __GAVEL_DEVICE_CMD_H