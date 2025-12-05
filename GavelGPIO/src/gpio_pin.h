
#ifndef __GAVEL_GPIO_PIN_H
#define __GAVEL_GPIO_PIN_H

#include "gpio_types.h"

#include <GavelInterfaces.h>

class GPIOPin {
public:
  GPIOPin(int physicalPin, IGPIOBackend* device, GpioConfig cfg, LedPolarity ledPol = LedPolarity::Source);

  bool setup();
  void tick();

  bool get();
  void set(bool v);
  unsigned int value() const;
  void setDuty(unsigned int pct);
  void setFreq(unsigned long hz);

  GpioType type() const { return cfg_.type; }
  int index() const { return cfg_.logicalIndex; }
  const char* note() const { return cfg_.note; }

private:
  int phys_;
  IGPIOBackend* device_;
  GpioConfig cfg_;
  LedPolarity ledPol_;
  bool cur_ = false;
  bool latchedButton_ = false;
  unsigned int duty_ = 0;
  unsigned long freq_ = 0;
  unsigned long pulseEndMs_ = 0;
};

#endif // __GAVEL_GPIO_PIN_H