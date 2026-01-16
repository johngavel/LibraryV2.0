#ifndef __GAVEL_DYNAMIC_FILE_H
#define __GAVEL_DYNAMIC_FILE_H

#include "digitalfile.h"

class DynamicFile : public DigitalFile {
public:
  DynamicFile(const char* name);
  DynamicFile(const char* name, FilePermission permission, char* buf, int size);
  virtual int size() override;
  virtual int read(unsigned char* buf, int __size) override;
  virtual operator bool() const override { return _isOpen; };
  virtual bool isOpen() const override { return _isOpen; };
  // DigitalBase virtuals
  virtual const char* name() const override;
  virtual bool open(FileMode mode = READ_MODE) override;
  virtual bool reset() override;
  virtual void close() override;
  virtual bool isDirectory() const override { return false; };

  // Stream virtuals
  virtual int available() override;
  virtual int read() override;
  virtual int peek() override;
  virtual void flush() override;
  virtual size_t write(const unsigned char* buffer, size_t __size) override;
  virtual size_t write(unsigned char) override;
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