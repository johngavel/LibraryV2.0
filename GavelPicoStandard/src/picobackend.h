
#ifndef __GAVEL_GPIO_INTERNAL_BACKEND_H
#define __GAVEL_GPIO_INTERNAL_BACKEND_H

#include <GavelInterfaces.h>

class RP2040Backend : public IGPIOBackend {
public:
  RP2040Backend();
  void setAvailablePins(BackendPinSetup* pinsetup) override;
  void setReservePins(BackendPinSetup* pinsetup) override;
  bool setupInput(int pin) override;
  bool setupOutput(int pin) override;
  bool setupAdc(int pin, unsigned char bits) override;
  bool readDigital(int pin) const override;
  void writeDigital(int pin, bool level) override;
  unsigned int readAnalog(int pin) const override;
  void pwmConfigure(int pin, unsigned long freqHz, unsigned int dutyPct) override;
  void toneStart(int pin, unsigned long freqHz) override;
  void toneStop(int pin) override;
};

#endif // __GAVEL_GPIO_INTERNAL_BACKEND_H