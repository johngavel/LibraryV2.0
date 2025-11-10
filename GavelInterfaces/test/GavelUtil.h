#ifndef __GAVEL_TEST_UTIL_H
#define __GAVEL_TEST_UTIL_H
char* getMacString(unsigned char* mac, char* buffer, unsigned long size) {
  return buffer;
};

class Mutex {};

class Identifiable {
public:
  Identifiable() : id(5) {}

  // Disable copy
  Identifiable(const Identifiable&) = delete;
  Identifiable& operator=(const Identifiable&) = delete;

protected:
  unsigned short id; // Unique ID for each object
};

#endif //