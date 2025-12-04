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

int safeCompare(const char* string1, const char* string2) {
  if (!isValidCString(string1)) return INT_MAX;
  if (!isValidCString(string2)) return INT_MIN;
  return (strcmp(string1, string2));
}

int safeCompare(const char* string1, const char* string2, int length) {
  if (!isValidCString(string1)) return INT_MAX;
  if (!isValidCString(string2)) return INT_MIN;
  return (strncmp(string1, string2, length));
}

char* safeAppend(char* dest, const char* src, int destSize) {
  if (!dest || !src || destSize == 0) return dest;

  int destLen = 0;
  while (destLen < destSize && dest[destLen] != '\0') ++destLen;
  if (destLen >= destSize) { // force terminate
    dest[destSize - 1] = '\0';
    destLen = destSize - 1;
  }
  int spaceLeft = destSize - destLen - 1;

  if (spaceLeft > 0) {
    // Copy up to spaceLeft (not relying on strncat semantics)
    int i = 0;
    for (; i < spaceLeft && src[i] != '\0'; ++i) dest[destLen + i] = src[i];
    dest[destLen + i] = '\0';
  }

  return dest;
}

#define MAX_TAB_LENGTH 32
char* tab(int tabLength, char* buffer, int size) {
  if (!buffer || tabLength <= 0 || size <= 0) return buffer;

  // Clamp tab length to prevent excessively wide tabs.
  int tabLen = tabLength;
  if (tabLen > MAX_TAB_LENGTH) tabLen = MAX_TAB_LENGTH;

  // Find current length within bounds (do not assume prior NUL).
  int curLen = 0;
  while (curLen < size && buffer[curLen] != '\0') ++curLen;
  if (curLen >= size) {
    // Force-terminate if not already NUL-terminated within 'size'.
    buffer[size - 1] = '\0';
    curLen = size - 1;
  }

  // Compute how many spaces to reach the next tab stop.
  int remainder = (tabLen > 0) ? (curLen % tabLen) : 0;
  int spaces = (remainder == 0) ? 0 : (tabLen - remainder);

  // Trim spaces if they don't fit.
  if (spaces + curLen >= size) spaces = size - curLen - 1;
  if (spaces < 0) spaces = 0;

  // Append all required spaces in one go.
  if (spaces > 0) {
    // Create a small local run of spaces (bounded).
    // For very large 'spaces', we can append in chunks.
    const int CHUNK = 32;
    char pad[CHUNK + 1];
    memset(pad, ' ', CHUNK);
    pad[CHUNK] = '\0';

    while (spaces > 0) {
      int n = (spaces > CHUNK) ? CHUNK : spaces;
      char saved = pad[n]; // temporarily terminate at 'n'
      pad[n] = '\0';
      safeAppend(buffer, pad, size);
      pad[n] = saved;
      spaces -= n;

      // Recompute curLen/remaining optional; safeAppend already guards NUL.
      // If you want to be extra safe, you could also break if buffer is full.
    }
  }
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
