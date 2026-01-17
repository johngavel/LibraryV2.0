#ifndef __GAVEL_ID_GENERATOR_H
#define __GAVEL_ID_GENERATOR_H

#include <string.h>

class IdGenerator {
private:
  unsigned short currentId;
  unsigned short startId;

public:
  explicit IdGenerator(unsigned short id) {
    currentId = id;
    startId = id;
  }
  unsigned short generateId() { return currentId++; }
  bool checkId(unsigned short id) { return ((id / 1000) == (startId / 1000)); }
};

// ID group bases
#define MEMORY_ID 2000
#define HARDWARE_ID 3000
#define IDLE_ID 4000
#define SYSTEM_ID 5000
#define TASK_ID 6000
#define MISC_ID 8000

// Global generators (defined once in a .cpp)

inline IdGenerator& hardwareID() noexcept {
  static IdGenerator g{HARDWARE_ID};
  return g;
}

inline IdGenerator& memoryID() noexcept {
  static IdGenerator g{MEMORY_ID};
  return g;
}

inline IdGenerator& idleID() noexcept {
  static IdGenerator g{IDLE_ID};
  return g;
}

inline IdGenerator& systemID() noexcept {
  static IdGenerator g{SYSTEM_ID};
  return g;
}

inline IdGenerator& taskID() noexcept {
  static IdGenerator g{TASK_ID};
  return g;
}

inline IdGenerator& miscID() noexcept {
  static IdGenerator g{MISC_ID};
  return g;
}

#define NAME_LENGTH 20

class Identifiable {
public:
  // Pass by reference to use the shared generator
  Identifiable() = delete;
  explicit Identifiable(IdGenerator& idGenerator, const char* _name) : id(idGenerator.generateId()) { setName(_name); };

  // Disable copy
  Identifiable(const Identifiable&) = delete;
  Identifiable& operator=(const Identifiable&) = delete;

  // Enable move
  Identifiable(Identifiable&&) noexcept = default;
  Identifiable& operator=(Identifiable&&) noexcept = default;

  unsigned short getId() const { return id; }
  void overrideID(unsigned short id_) { id = id_; }

  char* getName() { return name; };
  void setName(const char* __name) {
    memset(name, 0, NAME_LENGTH);
    strncpy(name, __name, NAME_LENGTH - 1);
  };

private:
  unsigned short id; // Unique ID for each object
  char name[20];
};

#endif // __GAVEL_ID_GENERATOR_H
