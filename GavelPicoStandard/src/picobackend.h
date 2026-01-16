#ifndef __GAVEL_GPIO_INTERNAL_BACKEND_H
#define __GAVEL_GPIO_INTERNAL_BACKEND_H

#include <GavelInterfaces.h>

class RP2040Backend : public IGPIOBackend {
public:
  RP2040Backend();
  virtual void start() override {};
  virtual void setAvailablePins(BackendPinSetup* pinsetup) override;
  virtual void setReservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupInput(int pin) override;
  virtual bool setupOutput(int pin) override;
  virtual bool setupAdc(int pin, unsigned char bits) override;
  virtual bool readDigital(int pin) const override;
  virtual void writeDigital(int pin, bool level) override;
  virtual unsigned int readAnalog(int pin) const override;
  virtual void pwmConfigure(int pin, unsigned long freqHz, unsigned int dutyPct) override;
  virtual void toneStart(int pin, unsigned long freqHz) override;
  virtual void toneStop(int pin) override;
  virtual bool isWorking() const override { return true; };
};

#endif // __GAVEL_GPIO_INTERNAL_BACKEND_H