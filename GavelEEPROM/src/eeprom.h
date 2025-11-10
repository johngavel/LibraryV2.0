#ifndef __GAVEL_EEPROM
#define __GAVEL_EEPROM

#include <GavelInterfaces.h>
#include <GavelProgram.h>
#include <GavelTask.h>
#include <GavelUtil.h>
#include <I2C_eeprom.h>
#include <Terminal.h>

#define MAX_DATA_SEGMENTS 10

class EEpromMemory : public Task {
public:
  EEpromMemory();
  void configure(unsigned long size);
  void addCmd(TerminalCommand* __termCmd) override;
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;
  void forceWrite();
  void setData(IMemory* __data) { dataList.push(&__data); };
  IMemory* getData(unsigned long index) { return (IMemory*) *((IMemory**) dataList.get(index)); };
  unsigned long getNumberOfData() { return dataList.count(); };
  unsigned long getLength();
  unsigned long getMemorySize() { return memorySize; };
  void reinitializeMemory();

private:
  void readEEPROM();
  void writeEEPROM();
  byte readEEPROMbyte(unsigned long address);
  void writeEEPROMbyte(unsigned long address, byte value);
  I2C_eeprom* i2c_eeprom = nullptr;
  ClassicQueue dataList = ClassicQueue(MAX_DATA_SEGMENTS, sizeof(IMemory*));
  unsigned long memorySize = 0;
  unsigned long dataSize = 0;
  OutputInterface* terminal;

  void wipe(OutputInterface* terminal);
  void mem(OutputInterface* terminal);
  void raw(OutputInterface* terminal);
};

#endif
