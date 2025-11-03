#include "debug.h"

#include <Arduino.h>
#include <GavelSerialPort.h>
#include <Terminal.h>

extern SerialPort serialPort;

void DEBUG(char* s) {
  if (serialPort.getSerial1Terminal()) serialPort.getSerial1Terminal()->println(TRACE, s);
  if (serialPort.getUSBSerialTerminal()) serialPort.getUSBSerialTerminal()->println(TRACE, s);
}
