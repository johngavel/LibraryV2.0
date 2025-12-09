#ifndef __GAVEL_GPIO_EXTERNAL_BACKEND_H
#define __GAVEL_GPIO_EXTERNAL_BACKEND_H

#include <GavelInterfaces.h>
class TCA9555; // forward

class Tca9555Backend : public IGPIOBackend {
public:
  explicit Tca9555Backend(unsigned char i2cAddr);
  ~Tca9555Backend();
  bool setupInput(int pin) override;
  bool setupOutput(int pin) override;
  bool setupAdc(int pin, unsigned char bits) override {
    (void) pin;
    (void) bits;
    return false;
  }
  bool readDigital(int pin) const override;
  void writeDigital(int pin, bool level) override;
  unsigned int readAnalog(int pin) const override {
    (void) pin;
    return 0;
  }

private:
  unsigned char addr_;
  TCA9555* dev_;
};

#endif // __GAVEL_GPIO_EXTERNAL_BACKEND_H