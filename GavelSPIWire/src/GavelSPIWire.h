#ifndef __GAVEL_SPI_WIRE_H
#define __GAVEL_SPI_WIRE_H

#include "clientUtil.h"

#include <GavelInterfaces.h>
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

  void reservePins(BackendPinSetup* pinsetup) {
    if (pinsetup != nullptr) {
      pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pinSCK, "SPI0 SCK");
      pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pinTX, "SPI0 TX");
      pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pinRX, "SPI0 RX");
      pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, pinCS, "SPI0 CS");
    }
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

#endif // __GAVEL_SPI_WIRE_H