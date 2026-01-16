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
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;

  GPIOPin* addPin(unsigned int deviceIdx, GpioConfig cfg, int pin, Polarity pol = Polarity::Source);
  GPIOPin* addPin(unsigned int deviceIdx, int pin, int logicalIndex, GpioType type, const char* note,
                  Polarity pol = Polarity::Source);

  virtual bool addReservePin(unsigned int deviceIdx, int pin, const char* note) override;
  virtual bool addAvailablePin(unsigned int deviceIdx, int pin) override;

  IGPIOBackend* addDevice(IGPIOBackend* device);

  GPIOPin* find(int deviceIdx, int pin);
  GPIOPin* find(GpioType type, int logicalIndex);

  void gpioTable(OutputInterface* terminal);
  void gpioTableStatus(OutputInterface* terminal);
  void statusCmd(OutputInterface* terminal);
  void pulseCmd(OutputInterface* terminal);
  void toneCmd(OutputInterface* terminal);
  void pwmCmd(OutputInterface* terminal);

private:
  IGPIOBackend* devices_[MAX_GPIO_DEVICES];
  ClassicSortList pins_ = ClassicSortList(MAX_PINS, sizeof(GPIOPin));
  bool initializePins = false;
};

#endif // __GAVEL_GPIO_MANAGER_H
