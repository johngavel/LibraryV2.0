#include "startup.h"

#include "filesystem.h"
#include "picobackend.h"

#include <GavelProgram.h>
#include <GavelUtil.h>

static Mutex startupMutex;
static Mutex startupMutex1;

PicoCommand pico;
TaskManager taskManager;
SerialPort serialPort;
Blink blink;
Watchdog watchdog;
FileSystem fileSystem;
GPIOManager gpioManager;
RP2040Backend picoDevice;

void setup0Start(TerminalCommand* __termCmd) {
  startupMutex.take();
  serialPort.configureUSBSerial();
  serialPort.getUSBSerialTerminal()->setBannerFunction(banner);
  StringBuilder sb = ProgramInfo::ShortName;
  sb + ":\\>";
  serialPort.getUSBSerialTerminal()->setPromptString(sb.c_str());

  taskManager.add(&serialPort);
  taskManager.add(&blink);
  taskManager.add(&watchdog);
  taskManager.add(&fileSystem);

  gpioManager.addDevice(0, &picoDevice);

  addStandardTerminalCommands(__termCmd);
  if (__termCmd) {
    __termCmd->addCmd("reboot", "", "Software Reboot the Pico", [&](TerminalLibrary::OutputInterface* terminal) { pico.rebootPico(terminal); });
    __termCmd->addCmd("upload", "", "Software Reboot the Pico into USB mode", [&](TerminalLibrary::OutputInterface* terminal) { pico.uploadPico(terminal); });
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
  serialPort.getMainSerialPort()->clearScreen();
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
