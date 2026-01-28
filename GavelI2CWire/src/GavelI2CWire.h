#ifndef __GAVEL_I2C_WIRE_H
#define __GAVEL_I2C_WIRE_H

#include <GavelInterfaces.h>
#include <GavelUtil.h>
#include <Wire.h>

class I2CWire {
public:
  I2CWire(){};
  void begin(unsigned long __pinSDA, unsigned long __pinSCL, bool isWire0 = true) {
    wireTake();
    pinSDA = __pinSDA;
    pinSCL = __pinSCL;
    if (isWire0) {
      wire = &Wire;
    } else {
      wire = &Wire1;
    }
    wire->setSDA(pinSDA);
    wire->setSCL(pinSCL);
    wire->begin();
    wireGive();
  }
  void reservePins(BackendPinSetup* pinsetup) {
    if (pinsetup != nullptr) {
      pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pinSDA, "I2c SDA");
      pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pinSCL, "I2c SCL");
    }
  }

  TwoWire* getWire() { return wire; };
  void wireTake() { lock.take(); };
  void wireGive() { lock.give(); };

private:
  TwoWire* wire;
  unsigned long pinSDA;
  unsigned long pinSCL;
  Mutex lock;
};

extern I2CWire i2cWire;

#endif // __GAVEL_I2C_WIRE_H