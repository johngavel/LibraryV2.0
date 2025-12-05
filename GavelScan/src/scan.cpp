#include "scan.h"

#include <GavelI2CWire.h>

#define MAX_SCAN_DEVICES 6

Scan::Scan() : Task("I2CScanner") {
  runTask(false);
};

void Scan::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("scan", "", "I2c Scanner", [this](TerminalLibrary::OutputInterface* terminal) { scani2c(terminal); });
}

void Scan::scani2c(OutputInterface* terminal) {
  byte error, address;
  int nDevices;

  terminal->println(INFO, "I2C Scanner");
  terminal->println(INFO, "Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    i2cWire.wireTake();
    i2cWire.getWire()->beginTransmission(address);
    error = i2cWire.getWire()->endTransmission();
    i2cWire.wireGive();

    if (error == 0) {
      terminal->print(INFO, "I2C device found at address 0x");
      if (address < 16) terminal->print(INFO, "0");
      terminal->print(INFO, String(address, HEX));
      terminal->println(INFO, "  !");
    } else if (error == 4) {
      terminal->print(WARNING, "Unknown error at address 0x");
      if (address < 16) terminal->print(INFO, "0");
      terminal->println(INFO, String(address, HEX));
    } else {
      // terminal->print(INFO, "No device found at address 0x");
      // if (address < 16) terminal->print(INFO, "0");
      // terminal->println(INFO, String(address, HEX));
    }
  }

  if (nDevices == 0)
    terminal->println(INFO, "No I2C devices found\n");
  else
    terminal->println(INFO, "done\n");
  terminal->prompt();
}
