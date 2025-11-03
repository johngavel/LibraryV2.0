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
  dataSize = sizeof(PrivateAppInfo) + ((getData()) ? getData()->getLength() : 0);
  if (memorySize == 0) { __terminal->println(ERROR, "EEPROM Memory Chip Unconfigured. "); }
  if (dataSize > fullDataSize) {
    __terminal->println(ERROR, "EEPROM Data Structure is too large: " + String(dataSize) + "/" + String(fullDataSize));
    dataSize = fullDataSize;
  }
  i2cWire.wireTake();
  //  i2c_eeprom = new I2C_eeprom(0x50, I2C_DEVICESIZE_24LC16, WIRE);
  i2c_eeprom = new I2C_eeprom(0x50, memorySize, &Wire);
  i2c_eeprom->begin();
  runTimer(i2c_eeprom->isConnected());
  i2cWire.wireGive();
  readEEPROM();
  if ((appInfo.ProgramNumber != ProgramInfo::ProgramNumber) || (appInfo.MajorVersion != ProgramInfo::MajorVersion)) {
    initMemory();
    __terminal->println(ERROR, "EEPROM memory incorrect values, intializing default values");
  } else {
    __terminal->println(PASSED, "EEPROM memory success");
  }
  if ((appInfo.MinorVersion != ProgramInfo::MinorVersion)) {
    appInfo.MajorVersion = ProgramInfo::MajorVersion;
    appInfo.MinorVersion = ProgramInfo::MinorVersion;
    breakSeal();
  }
  if (!getTimerRun()) __terminal->println(ERROR, "EEPROM Not Connected");
  String memoryString =
      "Memory Complete: PRG Num: " + String(appInfo.ProgramNumber) + " PRG Ver: " + String(appInfo.MajorVersion) + "." + String(appInfo.MinorVersion);
  if (getData())
    getData()->setup();
  else
    __terminal->println(WARNING, "No User Data Available!");
  __terminal->println((getTimerRun()) ? PASSED : FAILED, memoryString);
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

void EEpromMemory::initMemory() {
  appInfo.ProgramNumber = ProgramInfo::ProgramNumber;
  appInfo.MajorVersion = ProgramInfo::MajorVersion;
  appInfo.MinorVersion = ProgramInfo::MinorVersion;
  if (getData()) data->initMemory();
  breakSeal();
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
  unsigned long index = 0;
  unsigned char* appInfoData = (unsigned char*) &appInfo;
  if (getTimerRun()) {
    i2cWire.wireTake();
    mutex.take();
    for (index = 0; index < sizeof(PrivateAppInfo); index++) { appInfoData[index] = readEEPROMbyte(index); }
    if (getData()) {
      for (index = 0; index < data->getLength(); index++) { data->getData()[index] = readEEPROMbyte(index + sizeof(PrivateAppInfo)); }
    }
    mutex.give();
    i2cWire.wireGive();
  }
}

void EEpromMemory::writeEEPROM() {
  unsigned long index = 0;
  unsigned char* appInfoData = (unsigned char*) &appInfo;
  if (getTimerRun()) {
    i2cWire.wireTake();
    mutex.take();
    for (index = 0; index < sizeof(PrivateAppInfo); index++) { writeEEPROMbyte(index, appInfoData[index]); }
    if (getData()) {
      for (index = 0; index < data->getLength(); index++) { writeEEPROMbyte(index + sizeof(PrivateAppInfo), data->getData()[index]); }
    }
    mutex.give();
    i2cWire.wireGive();
  }
}

void EEpromMemory::wipe(OutputInterface* terminal) {
  initMemory();
  if (getData()) initMemory();
  terminal->println((getTimerRun()) ? PASSED : FAILED, "EEPROM Initialize Memory Complete.");
  terminal->prompt();
}

void EEpromMemory::mem(OutputInterface* terminal) {
  terminal->println(PROMPT, String(ProgramInfo::AppName) + " Version: " + String(ProgramInfo::MajorVersion) + String(".") + String(ProgramInfo::MinorVersion) +
                                String(".") + String(ProgramInfo::BuildVersion));
  terminal->println(INFO, "Program: " + String(ProgramInfo::ProgramNumber));
  terminal->println(INFO, "EEPROM Size: " + String(getLength()) + "/" + String(getMemorySize() / 8) + " bytes.");
  if (getData())
    getData()->printData(terminal);
  else
    terminal->println(WARNING, "No User Data Available!");
  terminal->println(INFO, "EEPROM Read Memory Complete");
  terminal->prompt();
}
