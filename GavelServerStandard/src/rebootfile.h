#ifndef __GAVEL_REBOOT_FILE_H
#define __GAVEL_REBOOT_FILE_H

#include <GavelFileSystem.h>
#include <GavelPicoStandard.h>

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

#endif // __GAVEL_REBOOT_FILE_H
