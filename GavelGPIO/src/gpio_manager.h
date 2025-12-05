
#ifndef __GAVEL_GPIO_MANAGER_H
#define __GAVEL_GPIO_MANAGER_H

#include "gpio_pin.h"

#include <GavelInterfaces.h>
#include <GavelTask.h>
#include <GavelUtil.h>

#define MAX_GPIO_DEVICES 10

class GPIOManager : public Task {
public:
  GPIOManager();
  void addCmd(TerminalCommand* __termCmd) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  GPIOPin* addPin(unsigned int deviceIdx, GpioConfig cfg, int pin, LedPolarity pol = LedPolarity::Source);

  IGPIOBackend* addDevice(unsigned int deviceIdx, IGPIOBackend* device);

  GPIOPin* find(GpioType type, int logicalIndex);

private:
  IGPIOBackend* devices_[MAX_GPIO_DEVICES];
  ClassicSortList pins_ = ClassicSortList(64, sizeof(GPIOPin));
};

#endif // __GAVEL_GPIO_MANAGER_H
