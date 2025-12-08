
#include "gpio_manager.h"

#include <GavelProgram.h>
#include <GavelUtil.h>
#include <asciitable/asciitable.h>

GPIOManager::GPIOManager() : Task("GPIOManager") {
  for (int i = 0; i < MAX_GPIO_DEVICES; i++) devices_[i] = nullptr;
}

GPIOPin* GPIOManager::addPin(unsigned int deviceIdx, GpioConfig cfg, int pin, LedPolarity pol) {
  return (addPin(deviceIdx, pin, cfg.logicalIndex, cfg.type, cfg.note, pol));
}

GPIOPin* GPIOManager::addPin(unsigned int deviceIdx, int pin, int logicalIndex, GpioType type, const char* note, LedPolarity pol) {
  if (deviceIdx >= MAX_GPIO_DEVICES) return nullptr;
  if (!devices_[deviceIdx]) return nullptr;
  GPIOPin* _pin = find(deviceIdx, pin);
  if ((_pin != nullptr) && (_pin->getConfig()->type == Available)) {
    _pin->getConfig()->type = type;
    _pin->getConfig()->logicalIndex = logicalIndex;
    strncpy(_pin->getConfig()->note, note, sizeof(_pin->getConfig()->note));
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
  __termCmd->addCmd("pulse", "[n]", "Command a Output n to pulse", [&](TerminalLibrary::OutputInterface* terminal) { pulseCmd(terminal); });
  __termCmd->addCmd("stat", "[n]", "Status of Input n", [&](TerminalLibrary::OutputInterface* terminal) { statusCmd(terminal); });
  __termCmd->addCmd("tone", "[n] [Hz]", "Sets a Square Wave in Hz on Tone Pin n ", [&](TerminalLibrary::OutputInterface* terminal) { toneCmd(terminal); });
  __termCmd->addCmd("pwm", "[n] [f] [%]", "Sets the frequency and % Duty Cycyle PWM Pin n", [&](TerminalLibrary::OutputInterface* terminal) { pwmCmd(terminal); });
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

void GPIOManager::pulseCmd(OutputInterface* terminal) {
  unsigned long index;
  GPIOPin* gpio;
  char* value;
  value = terminal->readParameter();
  if (value != NULL) {
    index = (unsigned long) atoi(value);
    gpio = find(Pulse, index);
    if (gpio != nullptr) {
      gpio->set(true);
    } else {
      terminal->println(ERROR, "Cannot find Pulse Pin.");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void GPIOManager::statusCmd(OutputInterface* terminal) {
  unsigned long index;
  GPIOPin* gpio;
  char* value;
  value = terminal->readParameter();
  if (value != NULL) {
    index = (unsigned long) atoi(value);
    gpio = find(Input, index);
    if (gpio != nullptr) {
      terminal->println(INFO, (gpio->get()) ? "ON" : "OFF");
    } else {
      terminal->println(ERROR, "Cannot find Input Pin.");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void GPIOManager::toneCmd(OutputInterface* terminal) {
  unsigned long freq;
  unsigned long index;
  GPIOPin* gpio;
  char* value;
  char* value2;
  value = terminal->readParameter();
  value2 = terminal->readParameter();
  if ((value != NULL) && (value2 != NULL)) {
    index = (unsigned long) atoi(value);
    freq = (unsigned long) atoi(value2);
    gpio = find(Tone, index);
    if (gpio != nullptr) {
      gpio->setFreq(freq);
    } else {
      terminal->println(ERROR, "Cannot find Tone Pin Index.");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}

void GPIOManager::pwmCmd(OutputInterface* terminal) {
  unsigned long frequency;
  unsigned long dutyCycle;
  unsigned long index;
  GPIOPin* gpio;
  char* value;
  char* value2;
  char* value3;
  value = terminal->readParameter();
  value2 = terminal->readParameter();
  value3 = terminal->readParameter();
  if ((value != NULL) && (value2 != NULL) && (value3 != NULL)) {
    index = (unsigned long) atoi(value);
    frequency = (unsigned long) atoi(value2);
    dutyCycle = (unsigned long) atoi(value3);
    gpio = find(Pwm, index);
    if (gpio != nullptr) {
      gpio->setFreq(frequency);
      gpio->setDuty(dutyCycle);
    } else {
      terminal->println(ERROR, "Cannot find PWM Pin Index.");
    }
  } else {
    terminal->invalidParameter();
  }
  terminal->prompt();
}