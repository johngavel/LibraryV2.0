#ifndef __GAVEL_GPIO_BACKEND_H
#define __GAVEL_GPIO_BACKEND_H

#define TASK_DEVICE_NAME_LENGTH 20

#include <GavelUtil.h>

#define MAX_PINS_DEVICE 64

#define GPIO_DEVICE_CPU_BOARD 0
#define GPIO_DEVICE_TCA9555 1

class BackendPinSetup {
public:
  virtual bool addReservePin(unsigned int deviceIdx, int pin, const char* note) = 0;
  virtual bool addAvailablePin(unsigned int deviceIdx, int pin) = 0;
};

class IGPIOBackend {
public:
  IGPIOBackend(char* name, int deviceIdx__) : _deviceIdx(deviceIdx__) { strncpy(_name, name, TASK_DEVICE_NAME_LENGTH); };
  virtual ~IGPIOBackend() = default;
  char* getDeviceName() { return _name; };
  int getDeviceIndex() { return _deviceIdx; };
  virtual void setAvailablePins(BackendPinSetup* pinsetup) = 0;
  virtual void setReservePins(BackendPinSetup* pinsetup) = 0;
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
  int _deviceIdx;
};

#endif // __GAVEL_GPIO_BACKEND_H