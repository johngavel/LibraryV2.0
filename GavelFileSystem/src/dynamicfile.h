#ifndef __GAVEL_DYNAMIC_FILE_H
#define __GAVEL_DYNAMIC_FILE_H

#include "digitalfile.h"

class DynamicFile : public DigitalFile {
public:
  DynamicFile(const char* name);
  DynamicFile(const char* name, FilePermission permission, char* buf, int size);
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
  void setBuffer(char* buf, int size) {
    _buffer = buf;
    _sizeBuffer = size;
  };
  char* getBuffer() { return _buffer; };
  int getBufferSize() { return _sizeBuffer; };

protected:
  int loadReadBuffer(const char* buffer, int size);
  int saveWriteBuffer(char* buf, int size);

private:
  virtual bool createReadData() = 0;
  virtual bool parseWriteData() = 0;

  char _name[200];
  int _cursor = 0;
  int _availableData = 0;
  bool _isOpen = false;
  char* _buffer = nullptr;
  int _sizeBuffer = 0;
};

#endif // __GAVEL_DYNAMIC_FILE_H