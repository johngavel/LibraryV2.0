#ifndef __GAVEL_HARDWARE_H
#define __GAVEL_HARDWARE_H

#include <GavelUtil.h>
#include <string.h>

class Hardware : public Identifiable {
public:
  // Explicitly forbid default construction
  Hardware() = delete;

  Hardware(const char* __name) : Identifiable(hardwareID(), __name){};
  virtual bool isWorking() const = 0;

private:
};

class HardwareList {
public:
  HardwareList() : count_(0) {}

  // Add-only, non-owning
  bool add(Hardware* hw) {
    if (!hw) return false;
    if (count_ >= 10) return false; // full
    items_[count_++] = hw;
    return true;
  }

  // Read-only inspection
  unsigned long long size() const noexcept { return count_; }
  bool empty() const noexcept { return count_ == 0; }
  bool full() const noexcept { return count_ == 10; }

  // Direct indexed access (non-owning)
  Hardware* operator[](unsigned long long index) const {
    if (index >= count_) return 0;
    return items_[index];
  }

private:
  Hardware* items_[10];      // non-owning pointers
  unsigned long long count_; // number of active items
};

#endif // __GAVEL_HARDWARE_H