
#ifndef __GAVEL_GPIO_BACKEND_H
#define __GAVEL_GPIO_BACKEND_H

#define TASK_DEVICE_NAME_LENGTH 20

#include <GavelUtil.h>

#define MAX_PINS_DEVICE 64

struct PinList {
  int pin[MAX_PINS_DEVICE];
  int numberOfPins;
};

class IGPIOBackend {
public:
  IGPIOBackend(char* name) { strncpy(_name, name, TASK_DEVICE_NAME_LENGTH); };
  virtual ~IGPIOBackend() = default;
  char* getDeviceName() { return _name; };
  virtual PinList* getAvailablePins() = 0;
  virtual bool setupInput(int pin) = 0;
  virtual bool setupOutput(int pin) = 0;
  virtual bool setupAdc(int pin, unsigned char bits = 12) = 0;
  virtual bool readDigital(int pin) const = 0;
  virtual void writeDigital(int pin, bool level) = 0;
  virtual unsigned int readAnalog(int pin) const = 0;

  // Optional features
  virtual void pwmConfigure(int pin, unsigned long freqHz, unsigned int dutyPct) {}
  virtual void toneStart(int pin, unsigned long freqHz) {}
  virtual void toneStop(int pin) {}

private:
  char _name[TASK_DEVICE_NAME_LENGTH];
};

#endif // __GAVEL_GPIO_BACKEND_H