#ifndef GAVEL_SPI_WIRE_H
#define GAVEL_SPI_WIRE_H

#include <GavelUtil.h>
#include <SPI.h>

class SPIWire {
public:
  SPIWire(){};
  void begin(unsigned long __pinSCK, unsigned long __pinTX, unsigned long __pinRX, unsigned long __pinCS) {
    pinSCK = __pinSCK;
    pinTX = __pinTX;
    pinRX = __pinRX;
    pinCS = __pinCS;

    SPI.setSCK(pinSCK); // 18
    SPI.setTX(pinTX);   // 19
    SPI.setRX(pinRX);   // 16
    SPI.setCS(pinCS);   // 17
    wireTake();
    SPI.begin();
    wireGive();
  }

  void wireTake() { lock.take(); };
  void wireGive() { lock.give(); };

private:
  unsigned long pinSCK;
  unsigned long pinTX;
  unsigned long pinRX;
  unsigned long pinCS;
  Mutex lock;
};

extern SPIWire spiWire;

#endif // GAVEL_SPI_WIRE_H