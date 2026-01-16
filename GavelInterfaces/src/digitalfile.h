#ifndef __GAVEL_DIGITAL_FILE_H
#define __GAVEL_DIGITAL_FILE_H

#include <Arduino.h>

enum FileMode { READ_MODE, WRITE_MODE };
enum FilePermission { READ_ONLY, WRITE_ONLY, READ_WRITE };

class DigitalBase {
public:
  virtual bool isDirectory() const = 0;
  virtual const char* name() const = 0;
  virtual bool open(FileMode mode = READ_MODE) = 0;
  virtual bool reset() = 0;
  virtual void close() = 0;
  virtual ~DigitalBase(){};
  void setPermission(FilePermission permission) { _permission = permission; };
  FilePermission getPermission() { return _permission; };
  FileMode getMode() { return _mode; };

protected:
  FilePermission _permission = READ_ONLY;
  FileMode _mode = READ_MODE;
};

class DigitalFile : public Stream, public DigitalBase {
public:
  virtual int size() = 0;
  virtual int read(unsigned char* buf, int __size) = 0;
  virtual operator bool() const = 0;
  virtual bool isOpen() const = 0;
  // DigitalBase virtuals
  virtual const char* name() const = 0;
  virtual bool open(FileMode mode = READ_MODE) = 0;
  virtual bool reset() = 0;
  virtual void close() = 0;
  virtual bool isDirectory() const override { return false; };

  // Stream virtuals
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;
  virtual size_t write(const unsigned char* buffer, size_t __size) = 0;
  virtual size_t write(unsigned char) = 0;

private:
};

class DigitalDirectory : public DigitalBase {
public:
  virtual DigitalBase* getNextFile() = 0;
  virtual void rewindDirectory() = 0;
  // DigitalBase virtuals
  virtual const char* name() const = 0;
  virtual bool open(FileMode mode = READ_MODE) override { return false; };
  virtual DigitalFile* open(const char* name, FileMode mode = READ_MODE) = 0;
  virtual DigitalDirectory* getDirectory(const char* name) = 0;
  virtual DigitalBase* getFile(const char* path) = 0;
  virtual bool reset() override {
    rewindDirectory();
    return true;
  };
  virtual void close() = 0;
  virtual bool isDirectory() const override { return true; };
  DigitalDirectory* getParent() { return _parentDir; };
  void setParent(DigitalDirectory* parent) { _parentDir = parent; };

private:
  DigitalDirectory* _parentDir = nullptr;
};

class DigitalFileSystem {
public:
  virtual DigitalBase* open(const char* path, FileMode mode = READ_MODE) = 0;
  virtual bool format() = 0;
  virtual bool verifyFile(const char* path) = 0;
  virtual DigitalFile* readFile(const char* path) = 0;
  virtual DigitalFile* writeFile(const char* path) = 0;
  virtual bool deleteFile(const char* path) = 0;

private:
};

#endif // __GAVEL_DIGITAL_FILE_H