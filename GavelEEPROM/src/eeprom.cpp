#include "eeprom.h"

#include <GavelI2CWire.h>

// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC16 / 8
// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC256 / 8

static char taskname[] = "EEPromMemory";
EEpromMemory::EEpromMemory() : Task(taskname) {}

void EEpromMemory::breakSeal() {
  seal = true;
}

void EEpromMemory::configure(unsigned long size) {
  memorySize = size;
}

void EEpromMemory::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("wipe", "", "Wipe and Initialize EEPROM Memory", [this](TerminalLibrary::OutputInterface* terminal) { wipe(terminal); });
  if (__termCmd) __termCmd->addCmd("mem", "", "Contents of Flash Memory", [this](TerminalLibrary::OutputInterface* terminal) { mem(terminal); });
}

bool EEpromMemory::setupTask(OutputInterface* __terminal) {
  unsigned int fullDataSize = memorySize / 8;
  terminal = __terminal;

  setRefreshMilli(1000);
  dataSize = 0;
  for (unsigned long i = 0; i < dataList.count(); i++) dataSize += ((IMemory*) dataList.get(i))->size();
  if (memorySize == 0) { terminal->println(ERROR, "EEPROM Memory Chip Unconfigured. "); }
  if (dataSize > fullDataSize) {
    terminal->println(ERROR, "EEPROM Data Structure is too large: " + String(dataSize) + "/" + String(fullDataSize));
    dataSize = fullDataSize;
  }
  i2cWire.wireTake();
  //  i2c_eeprom = new I2C_eeprom(0x50, I2C_DEVICESIZE_24LC16, WIRE);
  i2c_eeprom = new I2C_eeprom(0x50, memorySize, &Wire);
  i2c_eeprom->begin();
  runTimer(i2c_eeprom->isConnected());
  i2cWire.wireGive();
  readEEPROM();
  if (!getTimerRun()) terminal->println(ERROR, "EEPROM Not Connected");
  if (getNumberOfData() == 0) { terminal->println(WARNING, "No User Data Available!"); }
  return getTimerRun();
}

bool EEpromMemory::executeTask() {
  if (seal) {
    forceWrite();
  } else {
    setRefreshMilli(1000);
  }
  return true;
}

void EEpromMemory::forceWrite() {
  setRefreshMilli(5000);
  seal = false;
  reset();
  writeEEPROM();
}

unsigned long EEpromMemory::getLength() {
  return dataSize;
}

byte EEpromMemory::readEEPROMbyte(unsigned long address) {
  byte result = 0xFF;
  result = i2c_eeprom->readByte(address);
  return result;
}

void EEpromMemory::writeEEPROMbyte(unsigned long address, byte value) {
  if (i2c_eeprom->isConnected() && !i2c_eeprom->updateByteVerify(address, value))
    if (terminal) terminal->println(ERROR, "Error in Writing EEPROM " + String(address) + " = " + String(value, HEX));
}

void EEpromMemory::readEEPROM() {
  if (getTimerRun()) {
    i2cWire.wireTake();
    mutex.take();
    unsigned long eepromIndex = 0;
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = (IMemory*) dataList.get(dataIndex);
      for (unsigned long i = 0; i < data->size(); i++) (*data)[i] = readEEPROMbyte(eepromIndex + i);
      eepromIndex += data->size();
    }
    mutex.give();
    i2cWire.wireGive();
  }
}

void EEpromMemory::writeEEPROM() {
  if (getTimerRun()) {
    i2cWire.wireTake();
    mutex.take();
    unsigned long eepromIndex = 0;
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = (IMemory*) dataList.get(dataIndex);
      for (unsigned long i = 0; i < data->size(); i++) writeEEPROMbyte(eepromIndex + i, (*data)[i]);
      eepromIndex += data->size();
    }
    mutex.give();
    i2cWire.wireGive();
  }
}

void EEpromMemory::wipe(OutputInterface* terminal) {
  for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
    IMemory* data = (IMemory*) dataList.get(dataIndex);
    data->initMemory();
  }
  terminal->println((getTimerRun()) ? PASSED : FAILED, "EEPROM Initialize Memory Complete.");
  terminal->prompt();
}

void EEpromMemory::mem(OutputInterface* terminal) {
  terminal->println(INFO, "EEPROM Size: " + String(getLength()) + "/" + String(getMemorySize() / 8) + " bytes.");
  if (dataList.count() != 0) {
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = (IMemory*) dataList.get(dataIndex);
      data->printData(terminal);
    }
  } else {
    terminal->println(WARNING, "No User Data Available!");
  }
  terminal->println(INFO, "EEPROM Read Memory Complete");
  terminal->prompt();
}
