#ifndef __GAVEL_API_FILE_H
#define __GAVEL_API_FILE_H

#include "api.h"

#include <GavelFileSystem.h> // DigitalFile, FilePermission, FileMode
#include <GavelInterfaces.h> // JsonInterface
#include <GavelUtil.h>       // CharRingBuffer, etc.

class APIFile : public DigitalFile {
public:
  APIFile(API* mem, const char* fileName, FilePermission permission) : _memory(mem) {
    strncpy(_name, fileName, sizeof(_name) - 1);
    _name[sizeof(_name) - 1] = 0;
    setPermission(permission);
  }

  APIFile(API* mem, const char* fileName) : APIFile(mem, fileName, READ_ONLY) {}

  APIFile(const APIFile&) = delete;
  APIFile& operator=(const APIFile&) = delete;

  virtual ~APIFile() override {
    // Body destructor frees buffer automatically
  }

  virtual bool isAPI() const override { return true; }

  // ---------------- DigitalFile overrides ----------------
  virtual const char* name() const override { return _name; }

  virtual bool open(FileMode mode = READ_MODE) override {
    if (_isOpen) return false;
    if ((mode == READ_MODE) && (_permission == WRITE_ONLY)) return false;
    if ((mode == WRITE_MODE) && (_permission == READ_ONLY)) return false;

    _mode = mode;
    _isOpen = true;

    _memory->clear();
    return true;
  }

  bool processAPIRead() {
    if (_isOpen && (_mode == READ_MODE)) return createReadData();
    return false;
  }
  bool processAPIWrite() {
    if (_isOpen && (_mode == WRITE_MODE)) return parseWriteData();
    return false;
  }

  virtual void close() override {
    _isOpen = false;
    _memory->clear();
  }

  virtual bool reset() override { return true; }
  virtual bool isDirectory() const override { return false; }
  virtual bool isOpen() const override { return _isOpen; }
  virtual operator bool() const override { return _isOpen; }
  virtual int size() override { return _memory->body_.size(); }

  // ---------------- Stream-like API ----------------
  virtual int available() override { return _memory->body_.available(); }

  virtual int read(unsigned char* buf, int __size) override {
    if (!_isOpen) return -1;
    return _memory->body_.read(buf, __size);
  }

  virtual int read() override {
    if (!_isOpen) return -1;
    return _memory->body_.read();
  }

  virtual int peek() override {
    if (!_isOpen) return -1;
    return _memory->body_.peek();
  }

  virtual void flush() override { /* no-op */ }

  virtual size_t write(const unsigned char* buffer, size_t __size) override {
    if (!_isOpen) return 0;
    // Body::write uses unsigned int; casting is safe for embedded-sized writes
    return (size_t) _memory->body_.write(buffer, (unsigned int) __size);
  }

  virtual size_t write(unsigned char c) override {
    if (!_isOpen) return 0;
    return (size_t) _memory->body_.write(c);
  }

  void clear() { _memory->body_.clear(); }
  const char* contentType() { return "application/json"; };
  API* getAPI() { return _memory; };

private:
  // JsonFile-like hooks now operate through Body
  bool createReadData() {
    _memory->body_.clear();
    if (_memory) _memory->create(*this);
    return true;
  }

  bool parseWriteData() { return _memory ? _memory->parse(*this) : false; }

  // File state
  bool _isOpen = false;
  FileMode _mode = READ_MODE;
  char _name[200];

  // JSON memory interface
  API* _memory = NULL;
};

#endif // __GAVEL_API_FILE_H
