#ifndef __GAVEL_SERIAL_H
#define __GAVEL_SERIAL_H

#include <GavelTask.h>
#include <GavelUtil.h>
#include <Terminal.h>

class SerialPort : public Task {
public:
  SerialPort();
  void addCmd(TerminalCommand* __termCmd) override;
  void reservePins(BackendPinSetup* pinsetup) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  void configureSerial1(int __txPin, int __rxPin);
  void configureUSBSerial();
  OutputInterface* getMainSerialPort();
  OutputInterface* getSerial1Terminal() { return terminalSerial1; };
  OutputInterface* getUSBSerialTerminal() { return terminalUSB; };

private:
  Terminal* terminalSerial1 = nullptr;
  Terminal* terminalUSB = nullptr;
  int txPin, rxPin;
};

#endif // __GAVEL_SERIAL_H
