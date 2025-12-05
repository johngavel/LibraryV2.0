
#ifndef __GAVEL_GPIO_TYPES_H
#define __GAVEL_GPIO_TYPES_H

enum GpioType { Input, Output, Led, Button, Pulse, Pwm, Tone, Adc };

enum LedPolarity { Sink, Source };

struct GpioConfig {
  GpioType type;
  int logicalIndex; // user-facing index (e.g., 'PWM #3')
  char note[20];    // description
};

struct PwmSettings {
  unsigned long freqHz{100};
  unsigned int dutyPercent{0};
};

#endif // __GAVEL_GPIO_TYPES_H