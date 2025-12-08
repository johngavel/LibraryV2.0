#include "serialport.h"

#include <GavelUtil.h>

SerialPort::SerialPort() : Task("SerialPort"){};

void SerialPort::addCmd(TerminalCommand* __termCmd) {}

bool SerialPort::setupTask(OutputInterface* __terminal) {
  setRefreshMilli(10);
  return true;
}

void SerialPort::reservePins(BackendPinSetup* pinsetup) {
  if ((terminalSerial1 != nullptr) && (pinsetup != nullptr)) {
    pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, txPin, "TTY Serial1 TX");
    pinsetup->addReservePin(GPIO_DEVICE_CPU_BOARD, rxPin, "TTY Serial1 RX");
  }
}

bool SerialPort::executeTask() {
  if (terminalSerial1 != nullptr) terminalSerial1->loop();
  if (terminalUSB != nullptr) terminalUSB->loop();
  return true;
}

void SerialPort::configureSerial1(int __txPin, int __rxPin) {
  txPin = __txPin;
  rxPin = __rxPin;
  Serial1.setRX(__rxPin);
  Serial1.setTX(__txPin);
  Serial1.begin(115200);
  terminalSerial1 = new Terminal(&Serial1);
  terminalSerial1->setup();
  terminalSerial1->setColor(true);
  terminalSerial1->setTerminalName("TTY Serial Port 1");
}

void SerialPort::configureUSBSerial() {
  Serial.begin();
  terminalUSB = new Terminal(&Serial);
  terminalUSB->setup();
  terminalUSB->setColor(true);
  terminalUSB->setTerminalName("USB Serial Port");
}

OutputInterface* SerialPort::getMainSerialPort() {
  if (terminalSerial1 != nullptr) return terminalSerial1;
  if (terminalUSB != nullptr) return terminalUSB;
  return nullptr;
}
