#ifndef __GAVEL_STREAM_FILE_H
#define __GAVEL_STREAM_FILE_H

#include <GavelFileSystem.h>
#include <GavelUtil.h>

class StreamFile : public DigitalFile {
public:
  StreamFile(const char* name) {
    strncpy(_name, name, sizeof(_name) - 1);
    _name[sizeof(_name) - 1] = 0;
    setPermission(READ_ONLY);
  };
  StreamFile(const char* name, FilePermission permission) {
    strncpy(_name, name, sizeof(_name) - 1);
    _name[sizeof(_name) - 1] = 0;
    setPermission(permission);
  };
  int size() override { return ringBuffer.available(); };
  int read(unsigned char* buf, int __size) override { return ringBuffer.read(buf, __size); };
  operator bool() const override { return true; };
  // DigitalBase virtuals
  const char* name() const override { return _name; };
  bool open(FileMode mode = READ_MODE) override {
    if (_mode == READ_MODE) createReadData();
    return true;
  };
  void close() override {
    if (_mode == WRITE_MODE) parseWriteData();
  };
  bool isDirectory() const override { return false; };

  // Stream virtuals
  int available() override { return ringBuffer.available(); };
  int read() override { return ringBuffer.pop(); };
  int peek() override { return ringBuffer.peek(); };
  void flush() override {};
  size_t write(const unsigned char* buffer, size_t __size) override { return ringBuffer.write(buffer, __size); };
  size_t write(unsigned char c) override { return ringBuffer.push(c); };

protected:
private:
  virtual bool createReadData() = 0;
  virtual bool parseWriteData() = 0;

  char _name[200];
  CharRingBuffer<4096> ringBuffer;
};

#endif // __GAVEL_STREAM_FILE_H