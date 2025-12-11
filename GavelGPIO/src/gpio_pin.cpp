
#include "gpio_pin.h"

#include <GavelDebug.h>
// Provide millis() from your platform headers

extern unsigned long millis();

GPIOPin::GPIOPin(int physicalPin, IGPIOBackend* device, GpioConfig cfg, Polarity ledPol) : phys_(physicalPin), device_(device), cfg_(cfg), pol_(ledPol) {}

bool GPIOPin::setup() {
  bool value = false;
  timer_.setRefreshMilli(50);
  timer_.runTimer(false);
  switch (cfg_.type) {
  case GpioType::Input: return device_->setupInput(phys_);
  case GpioType::Button: return device_->setupInput(phys_);
  case GpioType::Output:
  case GpioType::Led:
  case GpioType::Pulse:
    value = device_->setupOutput(phys_);
    device_->writeDigital(phys_, (pol_ == Polarity::Source) ? false : true);
    return value;
  case GpioType::Adc: return device_->setupAdc(phys_, 12);
  case GpioType::Pwm:
    device_->setupOutput(phys_);
    device_->pwmConfigure(phys_, freq_, duty_);
    return true;
  case GpioType::Tone:
    device_->setupOutput(phys_);
    device_->toneStop(phys_);
    return true;
  default: return true;
  }
  return false;
}

void GPIOPin::tick() {
  bool raw = false;
  bool active = false;
  bool pressEdge = false;
  bool releaseEdge = false;
  switch (cfg_.type) {
  case GpioType::Input:
    cur_ = device_->readDigital(phys_);
    if (pol_ == Sink) cur_ = !cur_;
    break;
  case GpioType::Button:
    raw = device_->readDigital(phys_);
    active = (pol_ == Source) ? raw : !raw;
    pressEdge = (!prevActive_ && active);
    releaseEdge = (prevActive_ && !active);
    prevActive_ = active;

    if (pressEdge) { timer_.runTimer(true); }
    if (releaseEdge) {
      if (timer_.expired()) { latchedButton_ = true; }
      timer_.runTimer(false);
    }
    cur_ = active;
    break;
  case GpioType::Pulse:
    if (timer_.expired()) {
      timer_.runTimer(false);
      cur_ = false;
      device_->writeDigital(phys_, (pol_ == Polarity::Source) ? false : true);
    }
    break;
  case GpioType::Pwm:
  case GpioType::Tone:
  case GpioType::Led:
  case GpioType::Output:
  case GpioType::Adc: break;
  case GpioType::Available:
  case GpioType::Reserved: break;
  default: break;
  }
}

bool GPIOPin::get() {
  return (cfg_.type == GpioType::Adc) ? (value() > 0) : cur_;
}

bool GPIOPin::buttonPressed() {
  bool wasPressed = latchedButton_;
  latchedButton_ = false; // Clear the latch after reporting
  return wasPressed;
}

void GPIOPin::set(bool v) {
  switch (cfg_.type) {
  case GpioType::Output:
    cur_ = v;
    device_->writeDigital(phys_, (pol_ == Polarity::Source) ? v : !v);
    break;
  case GpioType::Led:
    cur_ = v;
    device_->writeDigital(phys_, (pol_ == Polarity::Source) ? v : !v);
    break;
  case GpioType::Pulse:
    timer_.runTimer(true);
    cur_ = true;
    device_->writeDigital(phys_, (pol_ == Polarity::Source) ? true : false);
    break;
  case GpioType::Tone:
    if (v)
      device_->toneStart(phys_, freq_);
    else
      device_->toneStop(phys_);
    cur_ = v;
    break;
  default: break;
  }
}

unsigned int GPIOPin::value() const {
  return (cfg_.type == GpioType::Adc) ? device_->readAnalog(phys_) : duty_;
}

void GPIOPin::setDuty(unsigned int pct) {
  duty_ = pct;
  if (cfg_.type == GpioType::Pwm) device_->pwmConfigure(phys_, freq_, duty_);
}

void GPIOPin::setFreq(unsigned long hz) {
  freq_ = hz;
  if (cfg_.type == GpioType::Pwm) device_->pwmConfigure(phys_, freq_, duty_);
  if (cfg_.type == GpioType::Tone && cur_) device_->toneStart(phys_, freq_);
}
