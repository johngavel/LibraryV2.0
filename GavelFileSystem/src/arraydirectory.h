#ifndef __GAVEL_ARRAY_DIRECTORY_CLASS_H
#define __GAVEL_ARRAY_DIRECTORY_CLASS_H

#include <GavelInterfaces.h>

#define MAX_FILES 100
class ArrayDirectory : public DigitalDirectory {
public:
  ArrayDirectory(const char* name);
  bool addFile(DigitalFile* file);
  bool addDirectory(const char* name);
  DigitalBase* getFile(const char* name);
  // DigitalDirectory virtuals
  DigitalBase* openNextFile() override;
  void rewindDirectory() override;
  // DigitalBase virtuals
  const char* name() const override;
  virtual DigitalFile* open(const char* name, FileMode mode = READ_MODE) override;
  virtual DigitalDirectory* getDirectory(const char* name) override;
  void close() override;

private:
  char _name[200];
  DigitalBase* _files[MAX_FILES];
  int _fileCount = 0;
  int _cursor = 0;
};

#endif // __GAVEL_ARRAY_DIRECTORY_CLASS_H
