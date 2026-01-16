#ifndef __GAVEL_GPIO_EXTERNAL_BACKEND_H
#define __GAVEL_GPIO_EXTERNAL_BACKEND_H

#include <GavelInterfaces.h>
class TCA9555; // forward

class Tca9555Backend : public IGPIOBackend {
public:
  explicit Tca9555Backend(unsigned char i2cAddr);
  ~Tca9555Backend();
  void start();
  virtual void setAvailablePins(BackendPinSetup* pinsetup) override;
  virtual void setReservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupInput(int pin) override;
  virtual bool setupOutput(int pin) override;
  virtual bool setupAdc(int pin, unsigned char bits) override {
    (void) pin;
    (void) bits;
    return false;
  }
  virtual bool readDigital(int pin) const override;
  virtual void writeDigital(int pin, bool level) override;
  virtual unsigned int readAnalog(int pin) const override {
    (void) pin;
    return 0;
  }
  virtual bool isWorking() const override { return (dev_) ? true : false; };

private:
  unsigned char addr_;
  TCA9555* dev_;
};

#endif // __GAVEL_GPIO_EXTERNAL_BACKEND_H