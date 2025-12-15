#include "dynamicfile.h"

#include <GavelUtil.h>

DynamicFile::DynamicFile(const char* name) {
  strncpy(_name, name, sizeof(_name) - 1);
  _name[sizeof(_name) - 1] = 0;
}

int DynamicFile::size() {
  if (!_isOpen) return 0;
  return _size;
}

int DynamicFile::read(unsigned char* buf, int __size) {
  int readBytes = 0;
  int availableBytes = 0;
  if ((!_isOpen) || (_cursor >= size()) || (_mode != READ_MODE)) return -1;
  availableBytes = available();
  readBytes = (availableBytes > __size) ? __size : availableBytes;
  memcpy(buf, &_buffer[_cursor], readBytes);
  _cursor += readBytes;
  return readBytes;
}

// DigitalBase virtuals
const char* DynamicFile::name() const {
  return _name;
}

bool DynamicFile::open(FileMode mode) {
  if (_isOpen) return false;
  _mode = mode;
  _isOpen = true;
  _cursor = 0;
  _size = sizeof(_buffer);
  memset(_buffer, 0, _size);
  if (_mode == READ_MODE) createData();
  return true;
}

void DynamicFile::close() {
  if (_mode == WRITE_MODE) parseData();
  _isOpen = false;
  _cursor = 0;
}

// Stream virtuals
int DynamicFile::available() {
  return (_isOpen) ? (size() - _cursor) : 0;
}

int DynamicFile::read() {
  if (!_isOpen || _mode != READ_MODE || _cursor >= size()) return -1;
  return _buffer[_cursor++];
}

int DynamicFile::peek() {
  if (!_isOpen || _mode != READ_MODE || _cursor >= size()) return -1;
  return _buffer[_cursor];
}

void DynamicFile::flush() {
  // No-op for RAM-only
}

size_t DynamicFile::write(const unsigned char* buffer, size_t __size) {
  int writeBytes = 0;
  unsigned int availableBytes = 0;
  if ((!_isOpen) || (_cursor >= size()) || (_mode != WRITE_MODE)) return 0;
  availableBytes = available();
  writeBytes = (availableBytes > __size) ? __size : availableBytes;
  memcpy(&_buffer[_cursor], buffer, writeBytes);
  _cursor += writeBytes;
  return writeBytes;
}

size_t DynamicFile::write(unsigned char ch) {
  if (!_isOpen || _mode != WRITE_MODE || _cursor >= size()) return 0;
  _buffer[_cursor] = ch;
  _cursor++;
  return 1;
}

int DynamicFile::loadBuffer(const char* buffer, size_t __size) {
  unsigned int availableBytes = 0;
  if ((!_isOpen) || (_mode != READ_MODE)) return 0;
  availableBytes = sizeof(_buffer) - 1;
  _size = (availableBytes > __size) ? __size : availableBytes;
  memcpy(_buffer, buffer, _size);
  _buffer[_size] = 0;
  _cursor = 0;
  return _size;
}

int DynamicFile::saveBuffer(unsigned char* buf, int __size) {
  int readBytes = 0;
  int availableBytes = 0;
  if ((!_isOpen) || (_mode != WRITE_MODE)) return 0;
  availableBytes = _cursor;
  readBytes = (availableBytes > __size) ? __size : availableBytes;
  memcpy(buf, _buffer, readBytes);
  return readBytes;
}
