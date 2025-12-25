#include "arraydirectory.h"

#include <GavelUtil.h>

ArrayDirectory::ArrayDirectory(const char* name) {
  memset(_name, 0, sizeof(_name));
  strncpy(_name, name, sizeof(_name) - 1);
};

bool ArrayDirectory::addFile(DigitalFile* file) {
  if (!file) return false;
  if (_fileCount >= MAX_FILES) return false;
  if (getFile(file->name())) return false;
  _files[_fileCount] = file;
  _fileCount++;
  return true;
}

bool ArrayDirectory::addDirectory(const char* name) {
  if (!name || !*name) return false;
  if (_fileCount >= MAX_FILES) return false;
  if (getFile(name) != nullptr) return false;
  ArrayDirectory* dir = new ArrayDirectory(name);
  dir->setParent(this);
  _files[_fileCount] = dir;
  _fileCount++;
  return true;
};

DigitalBase* ArrayDirectory::getFile(const char* name) {
  for (int i = 0; i < _fileCount; i++) {
    if (safeCompare(_files[i]->name(), name) == 0) return _files[i];
  }
  return nullptr;
}

DigitalBase* ArrayDirectory::openNextFile() {
  if (_cursor < _fileCount) return _files[_cursor++];
  return nullptr;
};

void ArrayDirectory::rewindDirectory() {
  _cursor = 0;
};

const char* ArrayDirectory::name() const {
  return _name;
};

DigitalFile* ArrayDirectory::open(const char* name, FileMode mode) {
  DigitalBase* base = getFile(name);
  if (!base) return nullptr;
  if (base->isDirectory()) return nullptr;
  DigitalFile* file = static_cast<DigitalFile*>(base);
  file->open(mode);
  return file;
};

DigitalDirectory* ArrayDirectory::getDirectory(const char* name) {
  DigitalBase* base = getFile(name);
  if (!base) return nullptr;
  if (!base->isDirectory()) return nullptr;
  return static_cast<DigitalDirectory*>(base);
}

void ArrayDirectory::close() {};
