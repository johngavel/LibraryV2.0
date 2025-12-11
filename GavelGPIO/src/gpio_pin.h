#ifndef __GAVEL_GPIO_PIN_H
#define __GAVEL_GPIO_PIN_H

#include "gpio_types.h"

#include <GavelInterfaces.h>
#include <GavelUtil.h>

class GPIOPin {
public:
  GPIOPin(int physicalPin, IGPIOBackend* device, GpioConfig cfg, Polarity ledPol = Polarity::Source);

  bool setup();
  void tick();

  bool get();
  bool buttonPressed();
  void set(bool v);
  unsigned int value() const;
  void setDuty(unsigned int pct);
  unsigned int getDuty() { return duty_; };
  void setFreq(unsigned long hz);
  unsigned long getFreq() { return freq_; };
  void setPol(Polarity __pol) { pol_ = __pol; };
  Polarity getPol() { return pol_; };

  GpioType type() const { return cfg_.type; };
  int index() const { return cfg_.logicalIndex; };
  int physical() const { return phys_; };
  IGPIOBackend* device() const { return device_; };
  const char* note() const { return cfg_.note; };
  GpioConfig* getConfig() { return &cfg_; };

private:
  int phys_;
  IGPIOBackend* device_;
  GpioConfig cfg_;
  Polarity pol_;
  bool cur_ = false;
  bool latchedButton_ = false;
  bool prevActive_ = false;
  unsigned int duty_ = 0;
  unsigned long freq_ = 0;
  unsigned long pulseEndMs_ = 0;
  Timer timer_;
};

#endif // __GAVEL_GPIO_PIN_H