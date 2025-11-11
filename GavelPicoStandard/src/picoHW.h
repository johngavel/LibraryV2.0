#ifndef __GAVEL_PICOHW_H
#define __GAVEL_PICOHW_H

#include <GavelGPIO.h>

#if defined ARDUINO_RASPBERRY_PI_PICO
class PicoHardware : public HardwareDescription {
public:
  PicoHardware() : name("Raspberry Pi Pico") {}
  PinList& getListofPins() override {
    PinList list;
    Pin pin;
    pin.pinNumber = 0;
    list.
  };

private:
}
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
class PicoHardware : public HardwareDescription {}
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
class PicoHardware : public HardwareDescription {}
#elif defined ARDUINO_GENERIC_RP2040
class PicoHardware : public HardwareDescription {}
#else
#error "This architecture does not support this Hardware!"
#endif

#endif // __GAVEL_PICOHW_H