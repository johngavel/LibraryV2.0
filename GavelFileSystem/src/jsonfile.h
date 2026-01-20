#ifndef __GAVEL_JSON_FILE_H
#define __GAVEL_JSON_FILE_H

#include "streamfile.h"

#include <GavelInterfaces.h>

class JsonFile : public StreamFile {
public:
  static const unsigned int DEFAULT_BUFFER_SIZE = 2048;
  static const unsigned int LARGE_BUFFER_SIZE = 16384;
  JsonFile(JsonInterface* mem, const char* fileName, FilePermission permission, unsigned int bufferSize)
      : StreamFile(fileName, permission, bufferSize), _memory(mem){};
  JsonFile(JsonInterface* mem, const char* fileName) : JsonFile(mem, fileName, READ_ONLY, DEFAULT_BUFFER_SIZE){};
  JsonFile(JsonInterface* mem, const char* fileName, FilePermission permission)
      : JsonFile(mem, fileName, permission, DEFAULT_BUFFER_SIZE){};

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
