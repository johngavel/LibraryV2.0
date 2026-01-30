#ifndef __GAVEL_STREAM_FILE_H
#define __GAVEL_STREAM_FILE_H

#include "filesystem.h"

#include <GavelUtil.h>

class StreamFile : public DigitalFile {
public:
  StreamFile(const char* name, FilePermission permission, unsigned int bufferSize)
      : _bufSize(bufferSize), _buffer(new unsigned char[_bufSize]), ringBuffer(_buffer, _bufSize) {
    strncpy(_name, name, sizeof(_name) - 1);
    _name[sizeof(_name) - 1] = 0;
    setPermission(permission);
  };

  StreamFile(const StreamFile&) = delete;
  StreamFile& operator=(const StreamFile&) = delete;

  virtual ~StreamFile() override {
    delete[] _buffer;
    _buffer = 0;
    _bufSize = 0;
  }

  virtual int size() override { return ringBuffer.available(); };
  virtual int read(unsigned char* buf, int __size) override {
    if (!_isOpen) return -1;
    return ringBuffer.read(buf, __size);
  };
  virtual operator bool() const override { return _isOpen; };
  virtual bool isOpen() const override { return _isOpen; };
  // DigitalBase virtuals
  virtual const char* name() const override { return _name; };
  virtual bool open(FileMode mode = READ_MODE) override {
    if (_isOpen) return false;
    if ((mode == READ_MODE) && (_permission == WRITE_ONLY)) return false;
    if ((mode == WRITE_MODE) && (_permission == READ_ONLY)) return false;
    _mode = mode;
    _isOpen = true;
    if (_mode == WRITE_MODE) ringBuffer.clear();
    if (_mode == READ_MODE) createReadData();
    return true;
  };

  virtual bool reset() override { return true; };

  virtual void close() override {
    if ((_isOpen) && (_mode == WRITE_MODE)) parseWriteData();
    _isOpen = false;
  };
  virtual bool isDirectory() const override { return false; };

  // Stream virtuals
  virtual int available() override { return ringBuffer.available(); };
  virtual int read() override {
    if (!_isOpen) return -1;
    return ringBuffer.pop();
  };
  virtual int peek() override {
    if (!_isOpen) return -1;
    return ringBuffer.peek();
  };
  virtual void flush() override {};
  virtual size_t write(const unsigned char* buffer, size_t __size) override {
    if (!_isOpen) return 0;
    return ringBuffer.write(buffer, __size);
  };
  virtual size_t write(unsigned char c) override {
    if (!_isOpen) return 0;
    return ringBuffer.push(c);
  };
  void clear() { ringBuffer.clear(); };

protected:
  unsigned int _bufSize;
  unsigned char* _buffer;
  CharRingBuffer ringBuffer;

private:
  virtual bool createReadData() = 0;
  virtual bool parseWriteData() = 0;

  bool _isOpen = false;
  char _name[200];
};

#endif // __GAVEL_STREAM_FILE_H