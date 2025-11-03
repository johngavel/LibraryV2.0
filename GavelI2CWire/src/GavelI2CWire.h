#ifndef GAVEL_I2C_WIRE_H
#define GAVEL_I2C_WIRE_H

#include <GavelUtil.h>
#include <Wire.h>

class I2CWire {
public:
  I2CWire(){};
  void begin(unsigned long __pinSDA, unsigned long __pinSCL) {
    wireTake();
    pinSDA = __pinSDA;
    pinSCL = __pinSCL;
    wire = &Wire;
    wire->setSDA(pinSDA);
    wire->setSCL(pinSCL);
    wire->begin();
    wireGive();
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

#endif // GAVEL_I2C_WIRE_H