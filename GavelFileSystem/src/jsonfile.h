#ifndef __GAVEL_JSON_FILE_H
#define __GAVEL_JSON_FILE_H

#include "streamfile.h"

#include <GavelInterfaces.h>

class JsonFile : public StreamFile {
public:
  JsonFile(JsonInterface* mem, const char* fileName, FilePermission permission = READ_ONLY)
      : StreamFile(fileName, permission), _memory(mem){};

  virtual bool createReadData() override {
    clear();
    _memory->create(*this);
    return true;
  };

  virtual bool parseWriteData() override { return _memory->parse(*this); };

private:
  JsonInterface* _memory;
};

#endif // __GAVEL_JSON_FILE_H
