#ifndef __GAVEL_REBOOT_FILE_H
#define __GAVEL_REBOOT_FILE_H

#include <GavelDebug.h>
#include <GavelFileSystem.h>

class RebootFile : public DynamicFile {
public:
  RebootFile() : DynamicFile("reboot", WRITE_ONLY, _fileBuffer, sizeof(_fileBuffer)){};
  bool createReadData() override {
    DEBUG("Reboot - Create Read Data");
    return true;
  };

  virtual bool parseWriteData() override {
    DEBUG("Reboot - Parse Write Data");
    return true;
  };

private:
  char _fileBuffer[200];
};

#endif // __GAVEL_REBOOT_FILE_H
