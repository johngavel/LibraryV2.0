#include "program.h"

#include <GavelUtil.h>
#include <Terminal.h>

#if defined ARDUINO_RASPBERRY_PI_PICO
const HW_TYPES ProgramInfo::hw_type = HW_RASPBERRYPI_PICO;
#elif defined ARDUINO_RASPBERRY_PI_PICO_W
const HW_TYPES ProgramInfo::hw_type = HW_RASPBERRYPI_PICOW;
#elif defined ARDUINO_WAVESHARE_RP2040_ZERO
const HW_TYPES ProgramInfo::hw_type = HW_RP2040_ZERO;
#elif defined ARDUINO_GENERIC_RP2040
const HW_TYPES ProgramInfo::hw_type = HW_GAVEL_MINI_PICO_ETH;
#else
#error "This architecture does not support this Hardware!"
#endif

#ifdef GAVEL_VER
#define GAVEL_VERSION GAVEL_VER
#else
#define GAVEL_VERSION 65535
#endif

const char* ProgramInfo::compileDate = __DATE__;
const char* ProgramInfo::compileTime = __TIME__;
const unsigned long ProgramInfo::BuildVersion = GAVEL_VERSION;

/**
 * Static API handler implemented using TinyJsonBuilder.
 * Matches: ApiCallbackFn signature declared in apicallback.h
 */

// ---- Static API handler (builds JSON with TinyJsonBuilder) ----
bool ProgramInfo::createReadData() {
  // Compose "major.minor.build"
  String version = String(MajorVersion) + "." + String(MinorVersion) + "." + String(BuildVersion);

  TinyJsonBuilder jb;
  jb.beginObject();
  jb.add("product", String(AppName));
  jb.add("shortName", String(ShortName));
  jb.add("author", String(AuthorName));

  jb.add("program", (int) ProgramNumber); // numeric
  jb.add("version", version);

  jb.add("build_date", String(compileDate));
  jb.add("build_time", String(compileTime));
  jb.add("device", String(stringHardware(hw_type)));

  jb.endObject();

  return loadReadBuffer(jb.str().c_str(), jb.str().length()); // finalize JSON
}

bool ProgramInfo::parseWriteData() {
  return false;
}

const char* stringHardware(HW_TYPES hw_type) {
  const char* hwString;
  switch (hw_type) {
  case HW_RP2040_ZERO: hwString = "Waveshare RP2040 Zero"; break;
  case HW_RASPBERRYPI_PICO: hwString = "Raspberry Pi Pico"; break;
  case HW_RASPBERRYPI_PICOW: hwString = "Raspberry Pi Pico W"; break;
  case HW_GAVEL_MINI_PICO_ETH: hwString = "Gavel Mini Pico - Ethernet"; break;
  default: hwString = "Unknown"; break;
  }
  return hwString;
}

void banner(OutputInterface* terminal) {
  if (!terminal) return;
  terminal->println();
  StringBuilder sb;
  char buffer[10];
  sb + ProgramInfo::AppName + " Version: " + ProgramInfo::MajorVersion + "." + ProgramInfo::MinorVersion + "." +
      ProgramInfo::BuildVersion;

  terminal->println(INFO, sb.c_str());
  sb.clear();
  terminal->print(INFO, "Program: ");
  terminal->println(INFO, numToA(ProgramInfo::ProgramNumber, buffer, sizeof(buffer)));
  sb + "Build Date: " + ProgramInfo::compileDate + " Time: " + ProgramInfo::compileTime;
  terminal->println(INFO, sb.c_str());
  sb.clear();
  terminal->println();
  sb + "Terminal: " + terminal->getTerminalName().c_str();
  terminal->println(INFO, sb.c_str());
  sb.clear();
  terminal->println();
  terminal->print(INFO, "Microcontroller: ");
  terminal->println(INFO, stringHardware(ProgramInfo::hw_type));
  terminal->print(INFO, "Core is running at ");
  terminal->print(INFO, numToA((long) (rp2040.f_cpu() / 1000000), buffer, sizeof(buffer)));
  terminal->println(INFO, " Mhz");
  int used = rp2040.getUsedHeap();
  int total = rp2040.getTotalHeap();
  int percentage = (used * 100) / total;
  terminal->print(INFO, "RAM Memory Usage: ");
  terminal->print(INFO, numToA(used, buffer, sizeof(buffer)));
  terminal->print(INFO, "/");
  terminal->print(INFO, numToA(total, buffer, sizeof(buffer)));
  terminal->print(INFO, " --> ");
  terminal->print(INFO, numToA(percentage, buffer, sizeof(buffer)));
  terminal->println(INFO, "%");
  terminal->print(INFO, "CPU Temperature: ");
  terminal->print(INFO, numToA((9.0 / 5.0 * analogReadTemp()) + 32.0, buffer, sizeof(buffer)));
  terminal->println(INFO, "°F.");
}
