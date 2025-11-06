#include "eeprom.h"

#include <GavelI2CWire.h>

// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC16 / 8
// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC256 / 8

static char taskname[] = "EEPromMemory";
EEpromMemory::EEpromMemory() : Task(taskname) {}

void EEpromMemory::configure(unsigned long size) {
  memorySize = size;
}

void EEpromMemory::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("wipe", "", "Wipe and Initialize EEPROM Memory", [this](TerminalLibrary::OutputInterface* terminal) { wipe(terminal); });
  if (__termCmd) __termCmd->addCmd("mem", "", "Contents of Flash Memory", [this](TerminalLibrary::OutputInterface* terminal) { mem(terminal); });
  if (__termCmd) __termCmd->addCmd("raw", "", "Raw Contents of Flash Memory", [this](TerminalLibrary::OutputInterface* terminal) { raw(terminal); });
}

bool EEpromMemory::setupTask(OutputInterface* __terminal) {
  unsigned int fullDataSize = memorySize / 8;
  terminal = __terminal;

  setRefreshMilli(1000);
  dataSize = 0;
  for (unsigned long i = 0; i < dataList.count(); i++) dataSize += (getData(i))->size();
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
  bool updated = false;
  for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
    IMemory* data = getData(dataIndex);
    if (data->getInternal()) { updated = true; }
  }

  if (updated) {
    forceWrite();
  } else {
    setRefreshMilli(1000);
  }
  return true;
}

void EEpromMemory::forceWrite() {
  setRefreshMilli(5000);
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
    unsigned long eepromIndex = 0;
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = getData(dataIndex);
      for (unsigned long i = 0; i < data->size(); i++) (*data)[i] = readEEPROMbyte(eepromIndex + i);
      data->setExternal(true);
      eepromIndex += data->size();
    }
    i2cWire.wireGive();
  }
}

void EEpromMemory::writeEEPROM() {
  if (getTimerRun()) {
    i2cWire.wireTake();
    unsigned long eepromIndex = 0;
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = getData(dataIndex);
      for (unsigned long i = 0; i < data->size(); i++) writeEEPROMbyte(eepromIndex + i, (*data)[i]);
      data->setInternal(false);
      eepromIndex += data->size();
    }
    i2cWire.wireGive();
  }
}

void EEpromMemory::wipe(OutputInterface* terminal) {
  for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
    IMemory* data = getData(dataIndex);
    data->initMemory();
    data->setExternal(true);
  }
  terminal->println((getTimerRun()) ? PASSED : FAILED, "EEPROM Initialize Memory Complete.");
  terminal->prompt();
}

void EEpromMemory::mem(OutputInterface* terminal) {
  terminal->println(INFO, "EEPROM Size: " + String(getLength()) + "/" + String(getMemorySize() / 8) + " bytes.");
  if (dataList.count() != 0) {
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = getData(dataIndex);
      data->printData(terminal);
      StringBuilder sb;
      sb + "Data Retrieved from EEPROM: " + data->getExternal();
      terminal->println(INFO, sb.c_str());
    }
  } else {
    terminal->println(WARNING, "No User Data Available!");
  }
  terminal->println();
  terminal->prompt();
}

#define BYTES_PER_LINE 16

static void hexLine(char* string, unsigned long stringSize, unsigned char* valueBuffer, unsigned long valueSize) {
  char hexString[4];
  if (((valueSize * 3) + 1) > stringSize) return;
  memset(string, 0, stringSize);
  StringBuilder sb;
  for (unsigned long i = 0; i < valueSize; i++) {
    hexByteString(valueBuffer[i], hexString, sizeof(hexString));
    sb + hexString + " ";
  }
  strncpy(string, sb.c_str(), stringSize);
}

static void printHexLine(OutputInterface* terminal, unsigned char* buffer, unsigned long lineNumber) {
  char string[80];
  StringBuilder sb;

  hexLine(string, sizeof(string), buffer, BYTES_PER_LINE);
  char hexLineNumber[4];
  hexByteString(lineNumber, hexLineNumber, sizeof(hexLineNumber));
  sb + "0x" + hexLineNumber + ": ";
  terminal->print(HELP, sb.c_str());
  terminal->println(INFO, string);
}

static unsigned char buffer[4096];
void EEpromMemory::raw(OutputInterface* terminal) {
  StringBuilder sb;
  memset(buffer, 0, 4096);
  unsigned long lines = (dataSize / BYTES_PER_LINE);
  unsigned long remainder = (dataSize % BYTES_PER_LINE);

  terminal->println(HELP, "EEPROM Raw Contents");
  sb + dataSize;
  terminal->println(HELP, "Data Size: ", sb.c_str());

  i2cWire.wireTake();
  for (unsigned long i = 0; i < dataSize; i++) buffer[i] = readEEPROMbyte(i);
  i2cWire.wireGive();

  for (unsigned long i = 0; i < lines; i++) { printHexLine(terminal, &buffer[i * BYTES_PER_LINE], i * BYTES_PER_LINE); }
  printHexLine(terminal, &buffer[lines], remainder);
  terminal->println();
  terminal->prompt();
}
