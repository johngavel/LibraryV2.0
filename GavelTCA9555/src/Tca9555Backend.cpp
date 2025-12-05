
#include "Tca9555Backend.h"

#include <GavelI2CWire.h>
#include <TCA9555.h>

static char devicename[] = "TaskManager";

Tca9555Backend::Tca9555Backend(unsigned char i2cAddr) : addr_(i2cAddr), dev_(nullptr) : IGPIOBackend(devicename) {
  i2cWire.wireTake();
  dev_ = new TCA9555(addr_, i2cWire.getWire()); // assumes global WIRE
  if (dev_) {
    dev_->begin();
    dev_->write16(0x00);
  }
  i2cWire.wireGive();
}

Tca9555Backend::~Tca9555Backend() {
  delete dev_;
}

static PinList availablePins;
PinList* InternalBackend::getAvailablePins() {
  availablePins.numberOfPins = 16;
  for (int i = 0; i < availablePins.numberOfPins; i++) availablePins.pin[i] = i;
  return &availablePins;
}

bool Tca9555Backend::setupInput(int pin) {
  if (!dev_) return false;
  i2cWire.wireTake();
  dev_->pinMode1(pin, INPUT);
  i2cWire.wireGive();
  return true;
}

bool Tca9555Backend::setupOutput(int pin) {
  if (!dev_) return false;
  i2cWire.wireTake();
  dev_->pinMode1(pin, OUTPUT);
  i2cWire.wireGive();
  return true;
}

bool Tca9555Backend::readDigital(int pin) const {
  i2cWire.wireTake();
  bool read = dev_ ? dev_->read1(pin) : false;
  i2cWire.wireGive();
  return read;
}

void Tca9555Backend::writeDigital(int pin, bool level) {
  i2cWire.wireTake();
  if (dev_) dev_->write1(pin, level);
  i2cWire.wireGive();
}
