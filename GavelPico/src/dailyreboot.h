#ifndef __GAVEL_DAILY_REBOOT_H
#define __GAVEL_DAILY_REBOOT_H

#include <GavelInterfaces.h>
#include <GavelTask.h>

class RebootTask : public Task {
public:
  RebootTask() : Task("ScheduleReboot"){};
  void configure(DeviceCmd* device) { _device = device; };
  virtual void addCmd(TerminalCommand* __termCmd) override {};
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override {
    setRefreshMinutes(1);
    _timer.setRefreshHours(24);
    return true;
  };
  bool loop();
  virtual bool executeTask() override {
    if (_device && _timer.expired()) { _device->reboot(); }
    return true;
  };

private:
  unsigned int delay_ms = 0;
  Timer _timer;
  DeviceCmd* _device = nullptr;
};

#endif // __GAVEL_DAILY_REBOOT_H