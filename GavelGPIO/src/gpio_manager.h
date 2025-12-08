
#ifndef __GAVEL_GPIO_MANAGER_H
#define __GAVEL_GPIO_MANAGER_H

#include "gpio_pin.h"

#include <GavelInterfaces.h>
#include <GavelTask.h>
#include <GavelUtil.h>

#define MAX_GPIO_DEVICES 10
#define MAX_PINS 64

class GPIOManager : public Task, public BackendPinSetup {
public:
  GPIOManager();
  void addCmd(TerminalCommand* __termCmd) override;
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  GPIOPin* addPin(unsigned int deviceIdx, GpioConfig cfg, int pin, LedPolarity pol = LedPolarity::Source);
  GPIOPin* addPin(unsigned int deviceIdx, int pin, int logicalIndex, GpioType type, const char* note, LedPolarity pol = LedPolarity::Source);

  bool addReservePin(unsigned int deviceIdx, int pin, const char* note) override;
  bool addAvailablePin(unsigned int deviceIdx, int pin) override;

  IGPIOBackend* addDevice(IGPIOBackend* device);

  GPIOPin* find(int deviceIdx, int pin);
  GPIOPin* find(GpioType type, int logicalIndex);

  void gpioTable(OutputInterface* terminal);
  void statusCmd(OutputInterface* terminal);
  void pulseCmd(OutputInterface* terminal);
  void toneCmd(OutputInterface* terminal);
  void pwmCmd(OutputInterface* terminal);

private:
  IGPIOBackend* devices_[MAX_GPIO_DEVICES];
  ClassicSortList pins_ = ClassicSortList(MAX_PINS, sizeof(GPIOPin));
};

#endif // __GAVEL_GPIO_MANAGER_H
