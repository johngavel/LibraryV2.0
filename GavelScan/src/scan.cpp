#include "scan.h"

#define MAX_SCAN_DEVICES 6

void Scan::setupTask(OutputInterface* terminal) {}

void Scan::addCmd(TerminalCommand* __termCmd) {
  TERM_CMD->addCmd("scan", "", "I2c Scanner", scanCmd);
}

void Scan::scani2c(OutputInterface* terminal) {
  byte error, address;
  int nDevices;
  String devicesFound[MAX_SCAN_DEVICES];

  terminal->println(INFO, "I2C Scanner");
  terminal->println(INFO, "Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    // COMM_TAKE;
    // WIRE->beginTransmission(address);
    // error = WIRE->endTransmission();
    // COMM_GIVE;

    if (error == 0) {
      terminal->print(INFO, "I2C device found at address 0x");
      if (address < 16) terminal->print(INFO, "0");
      terminal->print(INFO, String(address, HEX));
      terminal->println(INFO, "  !");

      if (nDevices < MAX_SCAN_DEVICES) { devicesFound[nDevices] = String(nDevices) + ". 0x" + String(address, HEX); }

      nDevices++;

    } else if (error == 4) {
      terminal->print(WARNING, "Unknown error at address 0x");
      if (address < 16) terminal->print(WARNING, "0");
      terminal->println(WARNING, String(address, HEX));
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

std::function<void(TerminalLibrary::OutputInterface*)> PicoCommand::scanCmd() {
  return std::bind(&PicoCommand::uploadPico, this, std::placeholders::_1);
}
