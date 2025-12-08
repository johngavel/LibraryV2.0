#include "gpio_types.h"

const char* gpioTypeToString(GpioType type) {
  switch (type) {
  case Input: return "Input";
  case Output: return "Output";
  case Led: return "Led";
  case Button: return "Button";
  case Pulse: return "Pulse";
  case Pwm: return "Pwm";
  case Tone: return "Tone";
  case Adc: return "Adc";
  case Reserved: return "Reserved";
  case Available: return "Available";
  default: return "Unknown";
  }
}

const char* gpioLedPolarityToString(LedPolarity pol) {
  switch (pol) {
  case Sink: return "Sink";
  case Source: return "Source";
  default: return "Unknown";
  }
}
