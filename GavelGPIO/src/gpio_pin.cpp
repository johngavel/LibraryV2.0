
#include "gpio_pin.h"
// Provide millis() from your platform headers

extern unsigned long millis();

GPIOPin::GPIOPin(int physicalPin, IGPIOBackend* device, GpioConfig cfg, LedPolarity ledPol) : phys_(physicalPin), device_(device), cfg_(cfg), ledPol_(ledPol) {}

bool GPIOPin::setup() {
  switch (cfg_.type) {
  case GpioType::Input:
  case GpioType::Button: return device_->setupInput(phys_);
  case GpioType::Output:
  case GpioType::Led:
  case GpioType::Pulse: return device_->setupOutput(phys_);
  case GpioType::Adc: return device_->setupAdc(phys_, 12);
  case GpioType::Pwm:
    device_->setupOutput(phys_);
    device_->pwmConfigure(phys_, freq_, duty_);
    return true;
  case GpioType::Tone:
    device_->setupOutput(phys_);
    device_->toneStop(phys_);
    return true;
  }
  return false;
}

void GPIOPin::tick() {
  switch (cfg_.type) {
  case GpioType::Input: cur_ = device_->readDigital(phys_); break;
  case GpioType::Button: {
    bool raw = device_->readDigital(phys_);
    static constexpr unsigned long debounceMs = 100;
    static unsigned long t0 = 0;
    if (!raw) {
      t0 = millis();
      latchedButton_ = false;
    } else if (!latchedButton_ && (millis() - t0) >= debounceMs) {
      latchedButton_ = true;
    }
    cur_ = raw;
    break;
  }
  case GpioType::Pulse:
    if (cur_ && millis() >= pulseEndMs_) {
      cur_ = false;
      device_->writeDigital(phys_, false);
    }
    break;
  case GpioType::Pwm:
  case GpioType::Tone:
  case GpioType::Led:
  case GpioType::Output:
  case GpioType::Adc: break;
  }
}

bool GPIOPin::get() {
  return (cfg_.type == GpioType::Adc) ? (value() > 0) : cur_;
}

void GPIOPin::set(bool v) {
  switch (cfg_.type) {
  case GpioType::Output:
    cur_ = v;
    device_->writeDigital(phys_, v);
    break;
  case GpioType::Led:
    cur_ = v;
    device_->writeDigital(phys_, (ledPol_ == LedPolarity::Source) ? v : !v);
    break;
  case GpioType::Pulse:
    cur_ = true;
    pulseEndMs_ = millis() + 100;
    device_->writeDigital(phys_, true);
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
