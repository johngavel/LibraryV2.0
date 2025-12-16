#ifndef __GAVEL_STATIC_FILE_H
#define __GAVEL_STATIC_FILE_H

#include "dynamicfile.h"

class StaticFile : public DynamicFile {
public:
  StaticFile(const char* name, const char* buf, int size) : DynamicFile(name, READ_ONLY, (char*) buf, size){};

private:
  virtual bool createReadData() override { return true; };
  virtual bool parseWriteData() override { return true; };
};

#endif // __GAVEL_STATIC_FILE_H