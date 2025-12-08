
#include "gpio_manager.h"

#include <GavelProgram.h>
#include <GavelUtil.h>
#include <asciitable/asciitable.h>

GPIOManager::GPIOManager() : Task("GPIOManager") {
  for (int i = 0; i < MAX_GPIO_DEVICES; i++) devices_[i] = nullptr;
}

GPIOPin* GPIOManager::addPin(unsigned int deviceIdx, GpioConfig cfg, int pin, LedPolarity pol) {
  if (deviceIdx >= MAX_GPIO_DEVICES) return nullptr;
  if (!devices_[deviceIdx]) return nullptr;
  GPIOPin* _pin = find(deviceIdx, pin);
  if ((_pin != nullptr) && (_pin->getConfig()->type == Available)) {
    _pin->getConfig()->type = cfg.type;
    _pin->getConfig()->logicalIndex = cfg.logicalIndex;
    strncpy(_pin->getConfig()->note, cfg.note, sizeof(_pin->getConfig()->note));
    _pin->setPol(pol);
  }
  return (_pin);
}

bool GPIOManager::addReservePin(unsigned int deviceIdx, int pin, const char* note) {
  if (deviceIdx >= MAX_GPIO_DEVICES) return false;
  if (!devices_[deviceIdx]) return false;
  GPIOPin* _pin = find(deviceIdx, pin);
  if (_pin != nullptr) {
    _pin->getConfig()->type = Reserved;
    strncpy(_pin->getConfig()->note, note, sizeof(_pin->getConfig()->note));
  }
  return (_pin != nullptr);
}

bool GPIOManager::addAvailablePin(unsigned int deviceIdx, int pin) {
  if (deviceIdx >= MAX_GPIO_DEVICES) return false;
  if (!devices_[deviceIdx]) return false;
  if (find(deviceIdx, pin) != nullptr) return false;

  GpioConfig cfg;
  cfg.type = Available;
  cfg.logicalIndex = pins_.count();
  strncpy(cfg.note, gpioTypeToString(cfg.type), sizeof(cfg.note));
  GPIOPin* _pin = new GPIOPin(pin, devices_[deviceIdx], cfg, LedPolarity::Source);
  return (pins_.push(_pin));
}

IGPIOBackend* GPIOManager::addDevice(IGPIOBackend* device) {
  if (device == nullptr) return nullptr;
  if (device->getDeviceIndex() >= MAX_GPIO_DEVICES) return nullptr;
  if (devices_[device->getDeviceIndex()] != nullptr) return nullptr;
  devices_[device->getDeviceIndex()] = device;
  device->setAvailablePins(this);
  device->setReservePins(this);
  return device;
}

GPIOPin* GPIOManager::find(int deviceIdx, int pin) {
  for (unsigned long i = 0; i < pins_.count(); i++) {
    GPIOPin* _pin = (GPIOPin*) pins_.get(i);
    if (_pin->device()->getDeviceIndex() == deviceIdx && _pin->physical() == pin) return _pin;
  }
  return nullptr;
}

GPIOPin* GPIOManager::find(GpioType type, int logicalIndex) {
  for (unsigned long i = 0; i < pins_.count(); i++) {
    GPIOPin* _pin = (GPIOPin*) pins_.get(i);
    if (_pin->type() == type && _pin->index() == logicalIndex) return _pin;
  }
  return nullptr;
}

void GPIOManager::addCmd(TerminalCommand* __termCmd) {
  __termCmd->addCmd("gpio", "", "Prints the configured GPIO Table", [&](TerminalLibrary::OutputInterface* terminal) { gpioTable(terminal); });
}

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

void GPIOManager::gpioTable(OutputInterface* terminal) {
  bool all = false;
  bool verbose = false;
  char* value = terminal->readParameter();

  if (value != nullptr) {
    if ((safeCompare("--all", value, 5) == 0) || (safeCompare("-a", value, 2) == 0)) {
      all = true;
      verbose = true;
    } else if ((safeCompare("--verbose", value, 9) == 0) || (safeCompare("-v", value, 2) == 0)) {
      verbose = true;
    } else {
      terminal->invalidParameter();
      terminal->prompt();
      return;
    }
  }

  // SORT LIST

  terminal->println(INFO, "GPIO Table");

  for (int i = 0; i < MAX_GPIO_DEVICES; i++) {
    if (devices_[i] != nullptr) {
      terminal->print(INFO, "Device: ");
      terminal->println(INFO, devices_[i]->getDeviceName());
    }
  }

  AsciiTable table(terminal);
  table.addColumn(Normal, "#", 4);
  table.addColumn(Green, "Type", 12);
  table.addColumn(Blue, "Pin", 5);
  table.addColumn(Normal, "Location", 14);
  table.addColumn(Magenta, "Index", 7);
  table.addColumn(Yellow, "Note", 30);
  table.printHeader();
  for (unsigned long i = 0; i < pins_.count(); i++) {
    GPIOPin* entry = (GPIOPin*) pins_.get(i);
    bool printPin = true;
    if ((entry->type() == Available) && (all == false)) printPin = false;
    if ((entry->type() == Reserved) && (verbose == false)) printPin = false;
    if (printPin) {
      char index[20], physical[20], logical[20];
      table.printData(numToA(i, index, 20), gpioTypeToString(entry->type()), numToA(entry->physical(), physical, 20), entry->device()->getDeviceName(),
                      numToA(entry->index(), logical, 20), entry->note());
    }
  }
  table.printDone("GPIO Table");

  terminal->println();
  terminal->prompt();
}