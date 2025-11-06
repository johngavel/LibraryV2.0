#include "stringbuilder.h"

#include "stringutils.h"

#include <cstring>

StringBuilder& StringBuilder::operator=(StringBuilder& sb) {
  clear();
  *this += sb;
  return *this;
}

StringBuilder& StringBuilder::operator=(bool b) {
  clear();
  *this += b;
  return *this;
}

StringBuilder& StringBuilder::operator=(const char* __string) {
  clear();
  *this += __string;
  return *this;
}

StringBuilder& StringBuilder::operator=(char c) {
  clear();
  *this += c;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned char c) {
  clear();
  *this += c;
  return *this;
}

StringBuilder& StringBuilder::operator=(long l) {
  clear();
  *this += l;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned long l) {
  clear();
  *this += l;
  return *this;
}

StringBuilder& StringBuilder::operator=(int i) {
  clear();
  *this += i;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned int i) {
  clear();
  *this += i;
  return *this;
}

StringBuilder& StringBuilder::operator=(short s) {
  clear();
  *this += s;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned short s) {
  clear();
  *this += s;
  return *this;
}

StringBuilder& StringBuilder::operator=(float f) {
  clear();
  *this += f;
  return *this;
}

StringBuilder& StringBuilder::operator=(double d) {
  clear();
  *this += d;
  return *this;
}

StringBuilder& StringBuilder::operator+=(StringBuilder& sb) {
  append_raw(sb.c_str());
  return *this;
}

StringBuilder& StringBuilder::operator+=(bool b) {
  if (b)
    append_raw("true");
  else
    append_raw("false");
  return *this;
}

StringBuilder& StringBuilder::operator+=(const char* __string) {
  append_raw(__string);
  return *this;
}

// Helper to append decimal for an 8-bit value 0..255
static inline StringBuilder& append_decimal_u8(StringBuilder& sb, unsigned int u8) {
  // u8 is 0..255; format once into a tiny stack buffer
  char buf[4]; // "255" + '\0'
  numToA((unsigned long) u8, buf, sizeof buf);
  return sb += buf;
}

// Always print decimal byte value 0..255
StringBuilder& StringBuilder::operator+=(char c) {
  unsigned int u8 = static_cast<int>(static_cast<unsigned char>(c));
  return append_decimal_u8(*this, u8);
}

StringBuilder& StringBuilder::operator+=(unsigned char c) {
  unsigned int u8 = static_cast<unsigned int>(c);
  return append_decimal_u8(*this, u8);
}

StringBuilder& StringBuilder::operator+=(long l) {
  char buf[32];
  numToA(l, buf, sizeof(buf));
  return append_raw(buf);
}

StringBuilder& StringBuilder::operator+=(unsigned long l) {
  char buf[32];
  numToA(l, buf, sizeof(buf));
  return append_raw(buf);
}

StringBuilder& StringBuilder::operator+=(int i) {
  long value = (long) i;
  *this += value;
  return *this;
}

StringBuilder& StringBuilder::operator+=(unsigned int i) {
  unsigned long value = (unsigned long) i;
  *this += value;
  return *this;
}

StringBuilder& StringBuilder::operator+=(short s) {
  long value = (long) s;
  *this += value;
  return *this;
}

StringBuilder& StringBuilder::operator+=(unsigned short s) {
  unsigned long value = (unsigned long) s;
  *this += value;
  return *this;
}

StringBuilder& StringBuilder::operator+=(float f) {
  double value = (double) f;
  *this += value;
  return *this;
}

StringBuilder& StringBuilder::operator+=(double d) {
  char buf[64];
  numToA(d, buf, sizeof(buf));
  *this += buf;
  return *this;
}

StringBuilder& StringBuilder::operator+(StringBuilder& sb) {
  *this += sb;
  return *this;
}

StringBuilder& StringBuilder::operator+(bool b) {
  *this += b;
  return *this;
}

StringBuilder& StringBuilder::operator+(const char* __string) {
  *this += __string;
  return *this;
}

StringBuilder& StringBuilder::operator+(char c) {
  *this += c;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned char c) {
  *this += c;
  return *this;
}

StringBuilder& StringBuilder::operator+(long l) {
  *this += l;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned long l) {
  *this += l;
  return *this;
}

StringBuilder& StringBuilder::operator+(int i) {
  *this += i;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned int i) {
  *this += i;
  return *this;
}

StringBuilder& StringBuilder::operator+(short s) {
  *this += s;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned short s) {
  *this += s;
  return *this;
}

StringBuilder& StringBuilder::operator+(float f) {
  *this += f;
  return *this;
}

StringBuilder& StringBuilder::operator+(double d) {
  *this += d;
  return *this;
}

const char* StringBuilder::c_str() const noexcept {
  return buffer_;
}

void StringBuilder::clear() noexcept {
  buffer_[0] = 0;
  size_ = 0;
}

int StringBuilder::size() const noexcept {
  return size_;
}

int StringBuilder::capacity() const noexcept {
  return kCapacity;
}

int StringBuilder::remaining() const noexcept {
  return kCapacity - size();
}

StringBuilder& StringBuilder::append_raw(const char* s) noexcept {
  if (!s) return *this;

  // Handle overlap (s points into our current buffer)
  if (s >= buffer_ && s <= buffer_ + size_) {
    const int offset = static_cast<int>(s - buffer_);
    const int tail = size_ - offset; // chars from s to '\0' (exclusive)
    char temp[STRINGBUILDER_MAX];
    std::memcpy(temp, s, tail);
    temp[tail] = '\0';
    s = temp;
  }

  int space = kCapacity - size_;
  while (*s && space-- > 0) buffer_[size_++] = *s++;
  buffer_[size_] = '\0';
  return *this;
}