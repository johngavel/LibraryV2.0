#ifndef __GAVEL_STREAM_FILE_H
#define __GAVEL_STREAM_FILE_H

#include <GavelFileSystem.h>
#include <GavelUtil.h>

class StreamFile : public DigitalFile {
public:
  StreamFile(const char* name) : ringBuffer(_ringBuffer, sizeof(_ringBuffer)) {
    strncpy(_name, name, sizeof(_name) - 1);
    _name[sizeof(_name) - 1] = 0;
    setPermission(READ_ONLY);
  };
  StreamFile(const char* name, FilePermission permission) : ringBuffer(_ringBuffer, sizeof(_ringBuffer)) {
    strncpy(_name, name, sizeof(_name) - 1);
    _name[sizeof(_name) - 1] = 0;
    setPermission(permission);
  };
  int size() override { return ringBuffer.available(); };
  int read(unsigned char* buf, int __size) override {
    if (!_isOpen) return -1;
    return ringBuffer.read(buf, __size);
  };
  operator bool() const override { return _isOpen; };
  bool isOpen() const override { return _isOpen; };
  // DigitalBase virtuals
  const char* name() const override { return _name; };
  bool open(FileMode mode = READ_MODE) override {
    if (_isOpen) return false;
    if ((mode == READ_MODE) && (_permission == WRITE_ONLY)) return false;
    if ((mode == WRITE_MODE) && (_permission == READ_ONLY)) return false;
    _mode = mode;
    _isOpen = true;
    if (_mode == WRITE_MODE) ringBuffer.clear();
    if (_mode == READ_MODE) createReadData();
    return true;
  };

  bool reset() override { return true; };

  void close() override {
    if ((_isOpen) && (_mode == WRITE_MODE)) parseWriteData();
    _isOpen = false;
  };
  bool isDirectory() const override { return false; };

  // Stream virtuals
  int available() override { return ringBuffer.available(); };
  int read() override {
    if (!_isOpen) return -1;
    return ringBuffer.pop();
  };
  int peek() override {
    if (!_isOpen) return -1;
    return ringBuffer.peek();
  };
  void flush() override {};
  size_t write(const unsigned char* buffer, size_t __size) override {
    if (!_isOpen) return 0;
    return ringBuffer.write(buffer, __size);
  };
  size_t write(unsigned char c) override {
    if (!_isOpen) return 0;
    return ringBuffer.push(c);
  };
  void clear() { ringBuffer.clear(); };

protected:
  CharRingBuffer ringBuffer;

private:
  virtual bool createReadData() = 0;
  virtual bool parseWriteData() = 0;

  bool _isOpen = false;
  char _name[200];
  unsigned char _ringBuffer[8192];
};

#endif // __GAVEL_STREAM_FILE_H