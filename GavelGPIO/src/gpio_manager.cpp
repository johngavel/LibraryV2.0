
#include "gpio_manager.h"

GPIOManager::GPIOManager() : Task("GPIOManager") {
  for (int i = 0; i < MAX_GPIO_DEVICES; i++) devices_[i] = nullptr;
}

GPIOPin* GPIOManager::addPin(unsigned int deviceIdx, GpioConfig cfg, int pin, LedPolarity pol) {
  if (deviceIdx > MAX_GPIO_DEVICES) return nullptr;
  if (!devices_[deviceIdx]) return nullptr;
  GPIOPin* _pin = new GPIOPin(pin, devices_[deviceIdx], cfg, pol);
  pins_.push(_pin);
  return _pin;
}

IGPIOBackend* GPIOManager::addDevice(unsigned int deviceIdx, IGPIOBackend* device) {
  if (deviceIdx > MAX_GPIO_DEVICES) return nullptr;
  if (!devices_[deviceIdx]) return nullptr;
  devices_[deviceIdx] = device;
  return device;
}

GPIOPin* GPIOManager::find(GpioType type, int logicalIndex) {
  for (unsigned long i = 0; i < pins_.count(); i++) {
    GPIOPin* _pin = (GPIOPin*) pins_.get(i);
    if (_pin->type() == type && _pin->index() == logicalIndex) return _pin;
  }
  return nullptr;
}

void addCmd(TerminalCommand* __termCmd) {}

bool GPIOManager::setupTask(OutputInterface* __terminal) {
  return true;
}

bool GPIOManager::executeTask() {
  for (unsigned long i = 0; i < pins_.count(); i++) {
    GPIOPin* _pin = (GPIOPin*) pins_.get(i);
    _pin->tick();
  }
  return true;
}
