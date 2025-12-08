#ifndef __GAVEL_ANALOG_DATA_H
#define __GAVEL_ANALOG_DATA_H

#define MAX_ANALOG 4095

typedef enum { UNKNOWN, HIGH_SIGNAL, LOW_SIGNAL, RISING_EDGE, FALLING_EDGE } SIGNAL;

typedef enum { CHANNEL_1, CHANNEL_2, MAX_CHANNEL } CHANNEL;

typedef struct {
  unsigned long timestamp;
  unsigned short raw[MAX_CHANNEL];
} RawSample;

typedef struct {
  unsigned long timestamp;
  unsigned short raw[MAX_CHANNEL];
  SIGNAL signal[MAX_CHANNEL];
} ProcessedSample;

#endif //__GAVEL_ANALOG_DATA_H