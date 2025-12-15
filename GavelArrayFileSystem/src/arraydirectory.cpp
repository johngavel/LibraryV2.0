#include "arrayfilesystem.h"

#include <GavelUtil.h>

ArrayDirectory::ArrayDirectory(const char* name) {
  memset(_name, 0, sizeof(_name));
  strncpy(_name, name, sizeof(_name) - 1);
};

bool ArrayDirectory::addFile(const char* name, const char* data, int __size) {
  if (!name || !*name || (!data && __size > 0)) return false;
  if (_fileCount >= MAX_FILES) return false;
  if (getFile(name) != nullptr) return false;
  ArrayFile* newFile = new ArrayFile();
  newFile->set(name, data, __size);
  if (!addFile(newFile)) {
    delete newFile;
    return false;
  };
  return true;
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
  _files[_fileCount] = new ArrayDirectory(name);
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

bool ArrayDirectory::open(FileMode mode) {
  return true;
};
void ArrayDirectory::close() {};
