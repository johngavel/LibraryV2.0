#include "eeprom.h"

#include <GavelI2CWire.h>

// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC16 / 8
// #define FULL_DATA_SIZE I2C_DEVICESIZE_24LC256 / 8
typedef struct {
  unsigned short id;
  unsigned short size;
  unsigned char spare[4];
} DataStruct;

typedef union {
  DataStruct dataStruct;
  unsigned char memoryBuffer[sizeof(DataStruct)];
} DataHeader;

EEpromMemory::EEpromMemory() : Task("EEPromMemory"), Hardware("EEPromMemory") {}

void EEpromMemory::configure(unsigned long size) {
  memorySize = size;
}

void EEpromMemory::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd)
    __termCmd->addCmd("wipe", "", "Wipe and Initialize EEPROM Memory",
                      [this](TerminalLibrary::OutputInterface* terminal) { wipe(terminal); });
  if (__termCmd)
    __termCmd->addCmd("mem", "", "Contents of Flash Memory",
                      [this](TerminalLibrary::OutputInterface* terminal) { mem(terminal); });
  if (__termCmd)
    __termCmd->addCmd("raw", "", "Raw Contents of Flash Memory",
                      [this](TerminalLibrary::OutputInterface* terminal) { raw(terminal); });
}

void EEpromMemory::reservePins(BackendPinSetup* pinsetup) {
  if (pinsetup != nullptr) { i2cWire.reservePins(pinsetup); }
}

bool EEpromMemory::setupTask(OutputInterface* __terminal) {
  StringBuilder sb;
  unsigned int fullDataSize = memorySize / 8;
  terminal = __terminal;

  setRefreshMilli(1000);
  dataSize = 0;
  for (unsigned long i = 0; i < dataList.count(); i++) {
    dataSize += sizeof(DataHeader);
    dataSize += (getData(i))->size();
  }
  if (memorySize == 0) { terminal->println(ERROR, "EEPROM Memory Chip Unconfigured. "); }
  if (dataSize > fullDataSize) {
    sb + "EEPROM Data Structure is too large: " + dataSize + "/" + fullDataSize;
    terminal->println(ERROR, sb.c_str());
    sb.clear();
    dataSize = fullDataSize;
  }
  i2cWire.wireTake();
  //  i2c_eeprom = new I2C_eeprom(0x50, I2C_DEVICESIZE_24LC16, WIRE);
  i2c_eeprom = new I2C_eeprom(0x50, memorySize, i2cWire.getWire());
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
  // if (terminal) terminal->println(WARNING, "Writing to EEPROM.");
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
  if (i2c_eeprom->isConnected() && !i2c_eeprom->updateByteVerify(address, value)) {
    StringBuilder sb;
    sb + "Error in Writing EEPROM " + address + " = " + value;
    if (terminal) terminal->println(ERROR, sb.c_str());
    sb.clear();
  }
}

void EEpromMemory::readEEPROM() {
  if (getTimerRun()) {
    i2cWire.wireTake();
    unsigned long eepromIndex = 0;
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      DataHeader dataHeader;
      IMemory* data = getData(dataIndex);
      for (unsigned long i = 0; i < sizeof(DataHeader); i++)
        dataHeader.memoryBuffer[i] = readEEPROMbyte(eepromIndex + i);
      eepromIndex += sizeof(DataHeader);
      if ((dataHeader.dataStruct.id == data->getId()) && (dataHeader.dataStruct.size == data->size())) {
        for (unsigned long i = 0; i < data->size(); i++) (*data)[i] = readEEPROMbyte(eepromIndex + i);
        data->updateExternal();
        eepromIndex += data->size();
      } else {
        StringBuilder sb;
        sb + "Invalid Data Segment in EEPROM <" + dataHeader.dataStruct.id + "/" + data->getId() + "> ";
        sb + "Size <" + dataHeader.dataStruct.size + "/" + data->size() + "> ";
        data->initMemory();
        data->updateExternal();
        terminal->println(ERROR, sb.c_str());
      }
    }
    i2cWire.wireGive();
  }
}

void EEpromMemory::writeEEPROM() {
  if (getTimerRun()) {
    i2cWire.wireTake();
    unsigned long eepromIndex = 0;
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      DataHeader dataHeader;
      memset(dataHeader.memoryBuffer, 0, sizeof(DataHeader));
      IMemory* data = getData(dataIndex);
      dataHeader.dataStruct.id = data->getId();
      dataHeader.dataStruct.size = data->size();
      for (unsigned long i = 0; i < sizeof(DataHeader); i++)
        writeEEPROMbyte(eepromIndex + i, dataHeader.memoryBuffer[i]);
      eepromIndex += sizeof(DataHeader);
      for (unsigned long i = 0; i < data->size(); i++) writeEEPROMbyte(eepromIndex + i, (*data)[i]);
      data->setInternal(false);
      eepromIndex += data->size();
    }
    i2cWire.wireGive();
  }
}

void EEpromMemory::reinitializeMemory() {
  for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
    IMemory* data = getData(dataIndex);
    data->initMemory();
    data->updateExternal();
  }
}

void EEpromMemory::wipe(OutputInterface* terminal) {
  reinitializeMemory();
  terminal->println((getTimerRun()) ? PASSED : FAILED, "EEPROM Initialize Memory Complete.");
  terminal->prompt();
}

void EEpromMemory::mem(OutputInterface* terminal) {
  StringBuilder sb;
  sb + "EEPROM Size: " + getLength() + "/" + (getMemorySize() / 8) + " bytes.";
  terminal->println(INFO, sb.c_str());
  sb.clear();
  if (dataList.count() != 0) {
    for (unsigned long dataIndex = 0; dataIndex < dataList.count(); dataIndex++) {
      IMemory* data = getData(dataIndex);
      sb = data->getName();
      sb + " Data ID: " + data->getId() + " Data Size: " + data->size();
      terminal->println(PROMPT, sb.c_str());
      sb.clear();
      data->printData(terminal);
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
  strncpy(string, sb.c_str(), stringSize - 1); // minimal safety: ensure null-termination
  string[stringSize - 1] = '\0';
}

// CHANGED: add 'count' so we can print less than BYTES_PER_LINE for the tail
static void printHexLine(OutputInterface* terminal, unsigned char* buffer, unsigned long lineNumber,
                         unsigned long count) {
  char string[80];
  StringBuilder sb;

  // CHANGED: use 'count' instead of always BYTES_PER_LINE
  hexLine(string, sizeof(string), buffer, count);

  char hexLineNumber[4];
  hexByteString(lineNumber, hexLineNumber, sizeof(hexLineNumber)); // kept as-is per "minimal"
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

  // Unchanged full lines, but pass the explicit count
  for (unsigned long i = 0; i < lines; i++) {
    printHexLine(terminal, &buffer[i * BYTES_PER_LINE], i * BYTES_PER_LINE, BYTES_PER_LINE);
  }

  // CHANGED: fix tail pointer and count
  if (remainder > 0) { printHexLine(terminal, &buffer[lines * BYTES_PER_LINE], lines * BYTES_PER_LINE, remainder); }

  terminal->println();
  terminal->prompt();
}