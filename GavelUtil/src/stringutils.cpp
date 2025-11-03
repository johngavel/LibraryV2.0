#include "stringutils.h"

#include <climits>
#include <cstring>
#include <ctype.h>
#include <stdio.h>

bool isValidCString(const char* str, unsigned int maxLen) {
  if (!str) return false;
  for (unsigned int i = 0; i < maxLen; ++i) {
    if (str[i] == '\0') return true; // Found terminator
  }
  return false; // No terminator within maxLen
}

int safeCompare(char* string1, char* string2) {
  if (!isValidCString(string1)) return INT_MAX;
  if (!isValidCString(string2)) return INT_MIN;
  return (strcmp(string1, string2));
}

char* safeAppend(char* dest, const char* src, int destSize) {
  if (!dest || !src || destSize == 0) return dest;

  int destLen = strlen(dest);
  int spaceLeft = (destSize > destLen) ? (destSize - destLen - 1) : 0;

  if (spaceLeft > 0) { strncat(dest, src, spaceLeft); }
  return dest;
}

char* tab(int tabLength, char* buffer, int size) {
  if (tabLength <= 0 || size <= 0) return buffer; // No space to write

  int totalLength = strlen(buffer);
  int remainder = totalLength % tabLength;
  int spaces = (remainder == 0) ? 0 : (tabLength - remainder);

  // Prevent buffer overflow
  if (spaces + totalLength >= size) spaces = size - totalLength - 1;
  if (spaces < 0) spaces = 0;

  for (int i = 0; i < spaces; i++) { safeAppend(buffer, " ", size); }
  return buffer;
}

#define MAX_TWO_DIGIT_LENGTH 3
char* hexByteString(unsigned char value, char* buffer, int size) {
  if (size < MAX_TWO_DIGIT_LENGTH) return buffer; // Safety check
  snprintf(buffer, size, "%02X", value);
  return buffer;
}

char* decByteString(unsigned char value, char* buffer, int size) {
  if (size < MAX_TWO_DIGIT_LENGTH) return buffer; // Safety check
  unsigned int capped_value = (value > 99) ? 99 : value;
  snprintf(buffer, size, "%02d", capped_value);
  return buffer;
}

#define MAX_MAC_STRING 18
char* getMacString(unsigned char* mac, char* buffer, int size) {
  char hexString[3];
  if (!mac || !buffer || size < MAX_MAC_STRING) return buffer; // Safety check
  memset(buffer, 0, size);
  for (int i = 0; i < 6; i++) {
    safeAppend(buffer, hexByteString(mac[i], (char*) &hexString, sizeof(hexString)), size);
    if (i < 5) safeAppend(buffer, ":", size);
  }
  return buffer;
}

#define MAX_IP_STRING 16
char* getIPString(unsigned char* ip, char* buffer, int size) {
  if (!ip || !buffer || size < MAX_IP_STRING) return buffer; // Safety check
  snprintf(buffer, size, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return buffer;
}

#define MAX_TIME_STRING 3
char* timeString(unsigned long __seconds, char* buffer, int size) {
  int hours = __seconds / 3600;
  char minutes[MAX_TIME_STRING];
  char seconds[MAX_TIME_STRING];
  decByteString((__seconds / 60) % 60, minutes, MAX_TIME_STRING);
  decByteString((__seconds % 60), seconds, MAX_TIME_STRING);
  snprintf(buffer, size, "%d:%s:%s", hours, minutes, seconds);
  return buffer;
}

char* trimWhitespace(char* buffer, int size) {
  if (!buffer || size <= 0) return buffer; // Safety check
  char* end;

  // Trim leading space
  while (isspace((unsigned char) *buffer)) buffer++;

  if (*buffer == 0) // All spaces?
    return buffer;

  // Trim trailing space
  end = buffer + strlen(buffer) - 1;
  while (end > buffer && isspace((unsigned char) *end)) end--;

  // Write new null terminator
  end[1] = '\0';

  return buffer;
}

char* numToA(int n, char* buffer, int size) {
  return numToA((long) n, buffer, size);
}

char* numToA(unsigned int n, char* buffer, int size) {
  return numToA((unsigned long) n, buffer, size);
}

char* numToA(char n, char* buffer, int size) {
  return numToA((long) n, buffer, size);
}

char* numToA(unsigned char n, char* buffer, int size) {
  return numToA((unsigned long) n, buffer, size);
}

char* numToA(short n, char* buffer, int size) {
  return numToA((long) n, buffer, size);
}

char* numToA(unsigned short n, char* buffer, int size) {
  return numToA((unsigned long) n, buffer, size);
}

char* numToA(long n, char* buffer, int size) {
  snprintf(buffer, size, "%ld", n);
  return buffer;
}

char* numToA(unsigned long n, char* buffer, int size) {
  snprintf(buffer, size, "%lu", n);
  return buffer;
}

char* numToA(float n, char* buffer, int size) {
  return numToA((double) n, buffer, size);
}

char* numToA(double n, char* buffer, int size) {
  snprintf(buffer, size, "%0.1f", n);
  return buffer;
}

StringBuilder& StringBuilder::operator=(char* __string) {
  clear();
  *this + __string;
  return *this;
}

StringBuilder& StringBuilder::operator=(const char* __string) {
  clear();
  *this + __string;
  return *this;
}

StringBuilder& StringBuilder::operator=(char c) {
  clear();
  *this + c;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned char c) {
  clear();
  *this + c;
  return *this;
}

StringBuilder& StringBuilder::operator=(long l) {
  clear();
  *this + l;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned long l) {
  clear();
  *this + l;
  return *this;
}

StringBuilder& StringBuilder::operator=(int i) {
  clear();
  *this + i;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned int i) {
  clear();
  *this + i;
  return *this;
}

StringBuilder& StringBuilder::operator=(short s) {
  clear();
  *this + s;
  return *this;
}

StringBuilder& StringBuilder::operator=(unsigned short s) {
  clear();
  *this + s;
  return *this;
}

StringBuilder& StringBuilder::operator=(float f) {
  clear();
  *this + f;
  return *this;
}

StringBuilder& StringBuilder::operator=(double d) {
  clear();
  *this + d;
  return *this;
}

StringBuilder& StringBuilder::operator+(char* __string) {
  safeAppend(string, __string, STRINGBUILDER_MAX);
  return *this;
}

StringBuilder& StringBuilder::operator+(const char* __string) {
  safeAppend(string, __string, STRINGBUILDER_MAX);
  return *this;
}

StringBuilder& StringBuilder::operator+(char c) {
  long value = (long) c;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned char c) {
  unsigned long value = (unsigned long) c;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(long l) {
  char buf[20];
  numToA(l, buf, sizeof(buf));
  *this + buf;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned long l) {
  char buf[20];
  numToA(l, buf, sizeof(buf));
  *this + buf;
  return *this;
}

StringBuilder& StringBuilder::operator+(int i) {
  long value = (long) i;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned int i) {
  unsigned long value = (unsigned long) i;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(short s) {
  long value = (long) s;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(unsigned short s) {
  unsigned long value = (unsigned long) s;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(float f) {
  double value = (double) f;
  *this + value;
  return *this;
}

StringBuilder& StringBuilder::operator+(double d) {
  char buf[20];
  numToA(d, buf, sizeof(buf));
  *this + buf;
  return *this;
}

char* StringBuilder::c_str() {
  return string;
}

void StringBuilder::clear() {
  memset(string, 0, STRINGBUILDER_MAX);
}
