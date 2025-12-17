#include "dynamicfile.h"

#include <string.h>

DynamicFile::DynamicFile(const char* name) {
  strncpy(_name, name, sizeof(_name) - 1);
  _name[sizeof(_name) - 1] = 0;
  setPermission(READ_ONLY);
  setBuffer(nullptr, 0);
}

DynamicFile::DynamicFile(const char* name, FilePermission permission, char* buf, int size) {
  strncpy(_name, name, sizeof(_name) - 1);
  _name[sizeof(_name) - 1] = 0;
  setPermission(permission);
  setBuffer(buf, size);
  _availableData = size;
}

int DynamicFile::size() {
  if (_mode == READ_MODE) return _availableData;
  if (_mode == WRITE_MODE) return _sizeBuffer;
  return 0;
}

int DynamicFile::read(unsigned char* buf, int __size) {
  int readBytes = 0;
  int availableBytes = 0;
  if (_buffer == nullptr || _sizeBuffer == 0) return -1;
  if ((!_isOpen) || (_cursor >= _sizeBuffer) || (_mode != READ_MODE) || (_permission == WRITE_ONLY)) return -1;
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
  if (_buffer == nullptr || _sizeBuffer == 0) return false;
  if ((mode == READ_MODE) && (_permission == WRITE_ONLY)) return false;
  if ((mode == WRITE_MODE) && (_permission == READ_ONLY)) return false;
  _mode = mode;
  _isOpen = true;
  _cursor = 0;
  if (_mode == WRITE_MODE) memset(_buffer, 0, _sizeBuffer);
  if (_mode == READ_MODE) createReadData();
  return true;
}

void DynamicFile::close() {
  if ((_mode == WRITE_MODE) && (_isOpen == true)) parseWriteData();
  _isOpen = false;
  _cursor = 0;
}

// Stream virtuals
int DynamicFile::available() {
  if (!_isOpen) return 0;
  if ((_mode == READ_MODE) && (_availableData > _cursor)) return _availableData - _cursor;
  if ((_mode == WRITE_MODE) && (_sizeBuffer > _cursor)) return _sizeBuffer - _cursor;
  return 0;
}

int DynamicFile::read() {
  if (_buffer == nullptr || _sizeBuffer == 0) return -1;
  if (!_isOpen || _mode != READ_MODE || _cursor >= _availableData) return -1;
  return _buffer[_cursor++];
}

int DynamicFile::peek() {
  if (_buffer == nullptr || _sizeBuffer == 0) return -1;
  if (!_isOpen || _mode != READ_MODE || _cursor >= _availableData) return -1;
  return _buffer[_cursor];
}

void DynamicFile::flush() {
  // No-op for RAM-only
}

size_t DynamicFile::write(const unsigned char* buffer, size_t __size) {
  int writeBytes = 0;
  unsigned int availableBytes = 0;
  if (_buffer == nullptr || _sizeBuffer == 0) return 0;
  if ((!_isOpen) || (_cursor >= size()) || (_mode != WRITE_MODE)) return 0;
  availableBytes = available();
  writeBytes = (availableBytes > __size) ? __size : availableBytes;
  memcpy(&_buffer[_cursor], buffer, writeBytes);
  _cursor += writeBytes;
  return writeBytes;
}

size_t DynamicFile::write(unsigned char ch) {
  if (_buffer == nullptr || _sizeBuffer == 0) return 0;
  if (!_isOpen || _mode != WRITE_MODE || _cursor >= size()) return 0;
  _buffer[_cursor] = ch;
  _cursor++;
  return 1;
}

int DynamicFile::loadReadBuffer(const char* buffer, int size) {
  if (_buffer == nullptr || _sizeBuffer == 0) return -1;
  if ((!_isOpen) || (_mode != READ_MODE)) return -1;
  _availableData = (_sizeBuffer > size) ? size : _sizeBuffer;
  memcpy(_buffer, buffer, _availableData);
  _cursor = 0;
  return _availableData;
}

int DynamicFile::saveWriteBuffer(char* buf, int size) {
  int readBytes = 0;
  if (_buffer == nullptr || _sizeBuffer == 0) return -1;
  if ((!_isOpen) || (_mode != WRITE_MODE)) return -1;
  readBytes = (_cursor > size) ? size : _cursor;
  memcpy(buf, _buffer, readBytes);
  return readBytes;
}
