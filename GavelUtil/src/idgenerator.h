#ifndef __GAVEL_ID_GENERATOR_H
#define __GAVEL_ID_GENERATOR_H

class IdGenerator {
private:
  static unsigned short currentId;

public:
  static unsigned short generateId() { return currentId++; }
};

inline unsigned short IdGenerator::currentId = 6000;

class Identifiable {
public:
  Identifiable() : id(IdGenerator::generateId()) {}

  // Disable copy
  Identifiable(const Identifiable&) = delete;
  Identifiable& operator=(const Identifiable&) = delete;

  // Enable move (explicitly defaulted so derived classes are moveable)
  Identifiable(Identifiable&&) noexcept = default;
  Identifiable& operator=(Identifiable&&) noexcept = default;

  unsigned short getId() const { return id; }
  void overrideID(unsigned short id_) { id = id_; };

protected:
  unsigned short id; // Unique ID for each object
};

#endif // __GAVEL_ID_GENERATOR_H