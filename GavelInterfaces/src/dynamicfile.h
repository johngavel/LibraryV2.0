#ifndef __GAVEL_DYNAMIC_FILE_H
#define __GAVEL_DYNAMIC_FILE_H

#include "digitalfile.h"

#define MAX_DIGITAL_FILE_BUFFER 1000

class DynamicFile : public DigitalFile {
public:
  DynamicFile(const char* name);
  int size() override;
  int read(unsigned char* buf, int __size) override;
  operator bool() const override { return _isOpen; };
  // DigitalBase virtuals
  const char* name() const override;
  bool open(FileMode mode = READ_MODE) override;
  void close() override;
  bool isDirectory() const override { return false; };

  // Stream virtuals
  int available() override;
  int read() override;
  int peek() override;
  void flush() override;
  size_t write(const unsigned char* buffer, size_t __size) override;
  size_t write(unsigned char) override;

protected:
  int loadBuffer(const char* buffer, size_t __size);
  int saveBuffer(unsigned char* buf, int __size);

private:
  virtual bool createData() = 0;
  virtual bool parseData() = 0;
  char _name[200];
  int _cursor = 0;
  int _size = 0;
  bool _isOpen = false;
  FileMode _mode = READ_MODE;
  char _buffer[MAX_DIGITAL_FILE_BUFFER];
};

#endif // __GAVEL_DYNAMIC_FILE_H