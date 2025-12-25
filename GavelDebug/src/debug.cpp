#include "debug.h"

#include <Arduino.h>
#include <GavelSerialPort.h>
#include <Terminal.h>

extern SerialPort serialPort;

void DEBUG(const char* s) {
  if (serialPort.getSerial1Terminal()) serialPort.getSerial1Terminal()->println(TRACE, s);
  if (serialPort.getUSBSerialTerminal()) serialPort.getUSBSerialTerminal()->println(TRACE, s);
}

void DEBUG(String s) {
  if (serialPort.getSerial1Terminal()) serialPort.getSerial1Terminal()->println(TRACE, s.c_str());
  if (serialPort.getUSBSerialTerminal()) serialPort.getUSBSerialTerminal()->println(TRACE, s.c_str());
}
