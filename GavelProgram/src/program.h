#ifndef __GAVEL_PROGRAM_H
#define __GAVEL_PROGRAM_H

#include <GavelInterfaces.h>
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

class ProgramMemory : public IMemory {
public:
  typedef struct {
    unsigned char ProgramNumber;
    unsigned char MajorVersion;
    unsigned char MinorVersion;
    unsigned char spare;
  } ProgramData;
  // 4 bytes
  static_assert(sizeof(ProgramData) == 4, "ProgramMemory size unexpected - check packing/padding.");

  typedef union {
    ProgramData data;
    unsigned char buffer[sizeof(ProgramData)];
  } ProgramUnion;

  ProgramMemory() { memset(memory.buffer, 0, sizeof(ProgramUnion::buffer)); };

  // IMemory overrides
  const unsigned char& operator[](std::size_t index) const override { return memory.buffer[index]; }
  unsigned char& operator[](std::size_t index) override {
    // Unchecked, like std::vector::operator[]
    return memory.buffer[index];
  }

  std::size_t size() const noexcept override { return sizeof(ProgramUnion::buffer); }

  void initMemory() override {
    memory.data.ProgramNumber = ProgramInfo::ProgramNumber;
    memory.data.MajorVersion = ProgramInfo::MajorVersion;
    memory.data.MinorVersion = ProgramInfo::MinorVersion;
    memory.data.spare = 0;
  }

  void printData(OutputInterface* terminal) override {
    StringBuilder sb;
    terminal->println(HELP, "Program ProgramUnion: ");
    sb + "Program: " + memory.data.ProgramNumber + " Version: " + memory.data.MajorVersion + "." + memory.data.MinorVersion;
    terminal->println(INFO, sb.c_str());
  }
  ProgramUnion memory;

private:
};

void banner(OutputInterface* terminal);

#endif // __GAVEL_PROGRAM_H