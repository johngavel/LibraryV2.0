#ifndef __GAVEL_LITTLEFS_FILE_H
#define __GAVEL_LITTLEFS_FILE_H

#include "digitalfile.h"

#include <Arduino.h>
#include <LittleFS.h>

// ===== Minimal AliasFile (wraps a LittleFS file, exposes a display name) =====
class AliasFile : public DigitalFile {
public:
  // displayName (e.g., "config.json"), physicalPath (e.g., "/config.json")
  AliasFile(const String& displayName, const String& physicalPath, FilePermission perm = READ_ONLY)
      : _display(displayName), _physical(physicalPath) {
    _file = File();
    _permission = perm;
  }

  ~AliasFile() override { close(); }

  // --- DigitalBase ---
  bool isDirectory() const override { return false; }
  const char* name() const override { return _display.c_str(); }

  bool open(FileMode mode = READ_MODE) override {
    // honor permission vs. requested mode
    if ((mode == READ_MODE && _permission == WRITE_ONLY) || (mode == WRITE_MODE && _permission == READ_ONLY)) {
      return false;
    }
    // open LittleFS file
    const char* fsMode = (mode == READ_MODE) ? "r" : "w";
    _file = LittleFS.open(_physical, fsMode);
    _mode = mode;
    return _file; // truthy if opened
  }

  bool reset() override {
    if (!_file) return false;
    return _file.seek(0);
  }

  void close() override {
    if (_file) { _file.close(); }
  }

  // --- DigitalFile specifics ---
  int size() override { return _file ? static_cast<int>(_file.size()) : 0; }
  int read(unsigned char* buf, int __size) override {
    if (!_file) return -1;
    return static_cast<int>(_file.read(buf, static_cast<size_t>(__size)));
  }

  operator bool() const override { return _file; }
  bool isOpen() const override { return _file; }

  // --- Stream ---
  int available() override { return _file ? _file.available() : 0; }
  int read() override { return _file ? _file.read() : -1; }
  int peek() override { return _file ? _file.peek() : -1; }
  void flush() override {
    if (_file) _file.flush();
  }
  size_t write(const unsigned char* buffer, size_t __size) override {
    if (!_file) return 0;
    return _file.write(buffer, __size);
  }
  size_t write(unsigned char b) override {
    if (!_file) return 0;
    return _file.write(b);
  }

private:
  String _display;
  String _physical;
  File _file;
};

// ===== Minimal AliasDirectory (fixed to two files) =====
class AliasDirectory : public DigitalDirectory {
public:
  explicit AliasDirectory(const String& displayPath) : _display(displayPath) {
    _aliases[0] = nullptr;
    _aliases[1] = nullptr;
    _count = 0;
    _cursor = 0;
  }

  ~AliasDirectory() override {
    for (int i = 0; i < _count; ++i) {
      if (_aliases[i]) {
        _aliases[i]->close();
        delete _aliases[i];
        _aliases[i] = nullptr;
      }
    }
    _count = 0;
  }

  // Add exactly two aliases total; returns false if already have two or duplicate name
  bool addAlias(const String& childDisplay, const String& childPhysical, FilePermission perm = READ_WRITE) {
    if (_count >= 2) return false;
    if (findAlias(childDisplay) != nullptr) return false;

    _aliases[_count] = new AliasFile(childDisplay, childPhysical, perm);
    ++_count;
    return true;
  }

  // --- Iteration over the two aliases ---
  DigitalBase* getNextFile() override {
    if (_cursor < _count) {
      AliasFile* f = _aliases[_cursor++];
      if (f && !f->isOpen()) f->open(READ_MODE); // ensure readable for listings
      return f;
    }
    return nullptr;
  }

  void rewindDirectory() override { _cursor = 0; }

  // --- DigitalBase ---
  const char* name() const override { return _display.c_str(); }
  bool open(FileMode mode = READ_MODE) override {
    (void) mode;
    return false;
  } // directory: no-op

  DigitalFile* open(const char* name, FileMode mode = READ_MODE) override {
    AliasFile* f = findAlias(String(name));
    if (!f) return nullptr;
    if (!f->isOpen()) {
      if (!f->open(mode)) return nullptr;
    }
    return f;
  }

  DigitalDirectory* getDirectory(const char* /*name*/) override {
    // No subdirectories in this minimal alias directory
    return nullptr;
  }

  DigitalBase* getFile(const char* name) override { return findAlias(String(name)); }

  bool reset() override {
    rewindDirectory();
    return true;
  }
  void close() override { /* container: no-op */ }
  bool isDirectory() const override { return true; }

private:
  AliasFile* findAlias(const String& display) const {
    for (int i = 0; i < _count; ++i) {
      if (_aliases[i] && display == String(_aliases[i]->name())) return _aliases[i];
    }
    return nullptr;
  }

  String _display;
  AliasFile* _aliases[2];
  int _count;
  int _cursor;
};

#endif // __GAVEL_LITTLEFS_FILE_H