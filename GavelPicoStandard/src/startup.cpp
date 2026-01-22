#include "startup.h"

#include "picobackend.h"

#include <GavelUtil.h>

static Mutex startupMutex;
static Mutex startupMutex1;

ProgramInfo programInfo;
ProgramMemory programMem;
PicoCommand pico;
TaskManager taskManager;
SerialPort serialPort;
Blink blink;
Watchdog watchdog;
FileSystem fileSystem;
GPIOManager gpioManager;
License license;
HardwareList hardwareList;
RP2040Backend rp2040Backend;

void setup0Start(TerminalCommand* __termCmd) {
  startupMutex.take();
#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
  license.addLibrary(ADAFRUIT_NEOPIXEL_INDEX);
#endif
  license.addLibrary(ARDUINO_CLI_INDEX);
  license.addLibrary(ARDUINO_IDE_INDEX);
  license.addLibrary(ARDUINO_PICO_INDEX);
  license.addLibrary(ARDUINOJSON_INDEX);
  license.addLibrary(ETHERNET_INDEX);
  license.addLibrary(I2C_EEPROM_INDEX);
  license.addLibrary(TERMINAL_INDEX);
  license.addLibrary(GAVEL_LIBRARIES_INDEX);

  gpioManager.addDevice(&rp2040Backend);

  serialPort.configureUSBSerial();
  serialPort.getUSBSerialTerminal()->setBannerFunction(banner);
  StringBuilder sb = ProgramInfo::ShortName;
  sb + ":\\>";
  serialPort.getUSBSerialTerminal()->setPromptString(sb.c_str());
  DBG_REGISTER(serialPort.getSerial1Terminal());
  DBG_REGISTER(serialPort.getUSBSerialTerminal());

  taskManager.add(&gpioManager);
  taskManager.add(&serialPort);
  taskManager.add(&blink);
  taskManager.add(&watchdog);
  taskManager.add(&fileSystem);
  taskManager.add(&license);

  hardwareList.add(&rp2040Backend);
  hardwareList.add(&blink);

  addStandardTerminalCommands(__termCmd);
  if (__termCmd) {
    __termCmd->addCmd("reboot", "", "Software Reboot the Pico",
                      [&](TerminalLibrary::OutputInterface* terminal) { pico.rebootPico(terminal); });
    __termCmd->addCmd("upload", "", "Software Reboot the Pico into USB mode",
                      [&](TerminalLibrary::OutputInterface* terminal) { pico.uploadPico(terminal); });
  }
}

void setup0SerialPort(int __txPin, int __rxPin) {
  serialPort.configureSerial1(__txPin, __rxPin);
  serialPort.getSerial1Terminal()->setBannerFunction(banner);
  StringBuilder sb = ProgramInfo::ShortName;
  sb + ":\\> ";
  serialPort.getSerial1Terminal()->setPromptString(sb.c_str());
}

void setup1Start() {
  startupMutex1.take();
}

void setup0Complete() {
  startupMutex.give();
  startupMutex1.take();
  taskManager.reservePins(&gpioManager);
  taskManager.setup(serialPort.getMainSerialPort());

  startupMutex1.give();
}

void setup1Complete() {
  startupMutex1.give();

  startupMutex.take();
  startupMutex.give();
}

void loop_0() {
  taskManager.loop();
}

void loop_1() {
  taskManager.loop();
}
