#include "arrayfilesystem.h"

ArrayFile::ArrayFile() {
  memset(_name, 0, sizeof(_name));
  _data = nullptr;
  _size = 0;
  _cursor = 0;
  _isOpen = false;
}

void ArrayFile::set(const char* name, char* data, int size) {
  strncpy(_name, name, sizeof(_name) - 1);
  _data = data;
  _size = size;
  _cursor = 0;
  _isOpen = false;
};

void ArrayFile::open() {
  if ((_size > 0) && (_data != nullptr)) {
    _isOpen = true;
    _cursor = 0;
  }
}

// Stream overrides
int ArrayFile::available() {
  return (_isOpen) ? (_size - _cursor) : 0;
}

int ArrayFile::read() {
  if (!_isOpen || _cursor >= _size) return -1;
  return _data[_cursor++];
}

int ArrayFile::peek() {
  if (!_isOpen || _cursor >= _size) return -1;
  return _data[_cursor];
}

void ArrayFile::flush() {
  // No-op for RAM-only
}

// File API methods
size_t ArrayFile::write(unsigned char b) {
  return 0;
}

size_t ArrayFile::write(const unsigned char* buf, size_t size) {
  return 0;
}

int ArrayFile::size() {
  return _size;
}

void ArrayFile::close() {
  _isOpen = false;
  _cursor = 0;
}

int ArrayFile::read(unsigned char* buf, int size) {
  int readBytes = 0;
  int availableBytes = 0;
  if (!_isOpen || _cursor >= _size) return -1;
  availableBytes = available();
  readBytes = (availableBytes > size) ? size : availableBytes;
  memcpy(buf, &_data[_cursor], readBytes);
  _cursor += readBytes;
  return readBytes;
}

ArrayFile::operator bool() const {
  return _isOpen;
}

const char* ArrayFile::name() const {
  return _name;
}
