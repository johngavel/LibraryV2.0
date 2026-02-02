#ifndef __GAVEL_STARTUP_H
#define __GAVEL_STARTUP_H

#include "picocmd.h"
#include "dailyreboot.h"

#include <Arduino.h>
#include <GavelBlinkPico.h>
#include <GavelFileSystem.h>
#include <GavelGPIO.h>
#include <GavelInterfaces.h>
#include <GavelLicense.h>
#include <GavelProgram.h>
#include <GavelSerialPort.h>
#include <GavelTaskManager.h>
#include <GavelWatchdog.h>

void setup0Start(TerminalCommand* __termCmd);
void setup0SerialPort(int __txPin, int __rxPin);
void setup1Start();
void setup0Complete();
void setup1Complete();
void loop_0();
void loop_1();

extern ProgramInfo programInfo;
extern ProgramMemory programMem;
extern PicoCommand pico;
extern TaskManager taskManager;
extern SerialPort serialPort;
extern Blink blink;
extern Watchdog watchdog;
extern GPIOManager gpioManager;
extern FileSystem fileSystem;
extern License license;
extern HardwareList hardwareList;

#endif // __GAVEL_STARTUP_H