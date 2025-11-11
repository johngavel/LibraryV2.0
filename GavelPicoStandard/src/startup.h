#ifndef __GAVEL_STARTUP_H
#define __GAVEL_STARTUP_H

#include <GavelBlinkPico.h>
#include <GavelGPIO.h>
#include <GavelPico.h>
#include <GavelSerialPort.h>
#include <GavelTask.h>
#include <GavelWatchdog.h>

void setup0Start(TerminalCommand* __termCmd);
void setup0SerialPort(int __txPin, int __rxPin);
void setup1Start();
void setup0Complete();
void setup1Complete();
void loop_0();
void loop_1();

extern PicoCommand pico;
extern TaskManager taskManager;
extern SerialPort serialPort;
extern Blink blink;
extern Watchdog watchdog;
extern GPIOManager gpioManager;

#endif // __GAVEL_STARTUP_H