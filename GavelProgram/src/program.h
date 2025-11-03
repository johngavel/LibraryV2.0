#ifndef __GAVEL_PROGRAM_H
#define __GAVEL_PROGRAM_H

#include <Terminal.h>

typedef enum { HW_UNKNOWN, HW_RP2040_ZERO, HW_RASPBERRYPI_PICO, HW_RASPBERRYPI_PICOW, HW_GAVEL_MINI_PICO_ETH } HW_TYPES;
const char* stringHardware(HW_TYPES hw_type);

class ProgramInfo {
public:
  static const char* AppName;
  static const char* ShortName;
  static const HW_TYPES hw_type;
  static const unsigned char ProgramNumber;
  static const unsigned char MajorVersion;
  static const unsigned char MinorVersion;
  static const unsigned long BuildVersion;
  static const char* compileDate;
  static const char* compileTime;
  static const char* AuthorName;
};

void banner(OutputInterface* terminal);

#endif // __GAVEL_PROGRAM_H