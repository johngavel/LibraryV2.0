#ifndef __GAVEL_REBOOT_FILE_H
#define __GAVEL_REBOOT_FILE_H

#include <GavelFileSystem.h>
#include <GavelPicoStandard.h>
#include <PicoOTA.h>

class RebootFile : public DynamicFile {
public:
  RebootFile() : DynamicFile("reboot.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)){};
  bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    pico.rebootPico();
    return true;
  };

private:
  char _fileBuffer[200];
};

class UpgradeFile : public DynamicFile {
public:
  UpgradeFile() : DynamicFile("upgrade.json", READ_WRITE, _fileBuffer, sizeof(_fileBuffer)){};
  bool createReadData() override { return true; };

  virtual bool parseWriteData() override {
    picoOTA.begin();
    picoOTA.addFile("pico.bin");
    picoOTA.commit();
    LittleFS.end();
    pico.rebootPico();
    return true;
  };

private:
  char _fileBuffer[200];
};

#endif // __GAVEL_REBOOT_FILE_H
