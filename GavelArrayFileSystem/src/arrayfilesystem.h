#ifndef __GAVEL_ARRAY_FILE_SYSTEM_CLASS_H
#define __GAVEL_ARRAY_FILE_SYSTEM_CLASS_H

#include <GavelInterfaces.h>

class ArrayFile : public DigitalFile {
public:
  ArrayFile();
  void set(const char* name, char* data, int size);

  // Digital File virtuals
  const char* name() const override;
  int size() override;
  void open() override;
  void close() override;
  int read(unsigned char* buf, int size) override;
  operator bool() const override;

  // Stream virtuals
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  size_t write(const uint8_t* buffer, size_t size) override;
  size_t write(uint8_t) override;

private:
  char _name[200];
  char* _data;
  int _size;
  int _cursor = 0;
  bool _isOpen = false;
};

#define MAX_FILES 100
class ArrayDirectory : public DigitalDirectory {
public:
  ArrayDirectory(const char* name);
  bool addFile(const char* name, char* data, int size);
  bool addDirectory(const char* name);
  DigitalBase* getFile(const char* name);
  // DigitalDirectory virtuals
  DigitalBase* openNextFile() override;
  void rewindDirectory() override;
  // DigitalBase virtuals
  const char* name() const override;
  void open() override;
  void close() override;

private:
  char _name[200];
  DigitalBase* _files[MAX_FILES];
  int _fileCount = 0;
  int _cursor = 0;
};

class ArrayFileSystem : public DigitalFileSystem {
public:
  ArrayFileSystem();
  DigitalBase* open(const char* path) override;
  bool format() override;
  bool verifyFile(const char* path) override;
  DigitalFile* readFile(const char* path) override;
  DigitalFile* writeFile(const char* path) override;
  bool deleteFile(const char* path) override;

private:
  DigitalBase* getFile(const char* path);
  ArrayDirectory* root;
};

#endif // __GAVEL_ARRAY_FILE_SYSTEM_CLASS_H
