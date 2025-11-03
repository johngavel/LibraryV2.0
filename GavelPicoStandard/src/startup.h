#ifndef __GAVEL_STARTUP
#define __GAVEL_STARTUP

#include <GavelBlinkPico.h>
#include <GavelPico.h>
#include <GavelSerialPort.h>
#include <GavelTask.h>

void setup0Start();
void setup0SerialPort(int __txPin, int __rxPin);
void setup1Start();
void setup0Complete();
void setup1Complete();

extern PicoCommand pico;
extern TaskManager taskManager;
extern SerialPort serialPort;
extern Blink blink;

#endif