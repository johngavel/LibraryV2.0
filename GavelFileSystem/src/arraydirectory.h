#ifndef __GAVEL_ARRAY_DIRECTORY_CLASS_H
#define __GAVEL_ARRAY_DIRECTORY_CLASS_H

#include <GavelInterfaces.h>

#define MAX_FILES 100
class ArrayDirectory : public DigitalDirectory {
public:
  ArrayDirectory(const char* name);
  bool addFile(DigitalFile* file);
  bool addDirectory(const char* name);
  bool addDirectory(DigitalDirectory* dir);
  virtual DigitalBase* getFile(const char* name) override;
  // DigitalDirectory virtuals
  virtual DigitalBase* getNextFile() override;
  virtual void rewindDirectory() override;
  // DigitalBase virtuals
  virtual const char* name() const override;
  virtual DigitalFile* open(const char* name, FileMode mode = READ_MODE) override;
  virtual DigitalDirectory* getDirectory(const char* name) override;
  virtual void close() override;

private:
  char _name[200];
  DigitalBase* _files[MAX_FILES];
  int _fileCount = 0;
  int _cursor = 0;
};

#endif // __GAVEL_ARRAY_DIRECTORY_CLASS_H
