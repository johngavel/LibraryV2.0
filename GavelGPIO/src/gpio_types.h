#ifndef __GAVEL_GPIO_TYPES_H
#define __GAVEL_GPIO_TYPES_H

#define MAX_ANALOG_VALUE 4096

enum GpioType { Input, Output, Led, Button, Pulse, Pwm, Tone, Adc, Reserved, Available };

enum Polarity { Sink, Source };

struct GpioConfig {
  GpioType type;
  int logicalIndex; // user-facing index (e.g., 'PWM #3')
  char note[20];    // description
};

struct PwmSettings {
  unsigned long freqHz{100};
  unsigned int dutyPercent{0};
};

const char* gpioTypeToString(GpioType type);
const char* gpioLedPolarityToString(Polarity pol);

#endif // __GAVEL_GPIO_TYPES_H