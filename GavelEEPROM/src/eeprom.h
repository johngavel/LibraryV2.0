#ifndef __GAVEL_EEPROM
#define __GAVEL_EEPROM

#include <GavelProgram.h>
#include <GavelTask.h>
#include <GavelUtil.h>
#include <I2C_eeprom.h>
#include <Terminal.h>

class Data {
public:
  virtual void setup() = 0;
  virtual void initMemory() = 0;
  virtual void printData(OutputInterface* terminal) = 0;
  virtual unsigned char* getData() = 0;
  virtual unsigned long getLength() = 0;
  virtual void exportMem() = 0;
  virtual void importMem() = 0;
};

class EEpromMemory : public Task {
public:
  EEpromMemory();
  void configure(unsigned long size);
  virtual void addCmd(TerminalCommand* __termCmd);
  virtual bool setupTask(OutputInterface* __terminal);
  virtual bool executeTask();
  void forceWrite();
  void breakSeal();
  bool getSeal() { return seal; };
  void initMemory();
  void setData(Data* __data) { data = __data; };
  Data* getData() { return data; };
  unsigned long getLength();
  unsigned long getMemorySize() { return memorySize; };
  Mutex* getMemoryLock() { return &mutex; };

private:
  void readEEPROM();
  void writeEEPROM();
  bool seal = false;
  byte readEEPROMbyte(unsigned long address);
  void writeEEPROMbyte(unsigned long address, byte value);
  I2C_eeprom* i2c_eeprom = nullptr;
  Data* data = nullptr;
  unsigned int memorySize = 0;
  unsigned int dataSize = 0;
  Mutex mutex;
  OutputInterface* terminal;

  struct PrivateAppInfo {
    unsigned char ProgramNumber = 0;
    unsigned char MajorVersion = 0;
    unsigned char MinorVersion = 0;
  };
  PrivateAppInfo appInfo;

  void wipe(OutputInterface* terminal);
  void mem(OutputInterface* terminal);
};

#endif
