#ifndef __GAVEL_HARDWARE_H
#define __GAVEL_HARDWARE_H

#include "json_interface.h"

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

class HardwareList : public JsonInterface {
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
  unsigned int size() const noexcept { return count_; }
  bool empty() const noexcept { return count_ == 0; }
  bool full() const noexcept { return count_ == 10; }

  // Direct indexed access (non-owning)
  Hardware* operator[](unsigned int index) const {
    if (index >= count_) return 0;
    return items_[index];
  }

  virtual JsonDocument createJson() override {
    JsonDocument doc;
    doc["numberhw"] = size();
    JsonArray data = doc["hwtable"].to<JsonArray>();
    for (unsigned int i = 0; i < size(); i++) {
      JsonObject object = data.add<JsonObject>();
      object["hwname"] = items_[i]->getName();
      object["hwid"] = items_[i]->getId();
      object["hwstatus"] = items_[i]->isWorking();
    }
    return doc;
  };

  virtual bool parseJson(JsonDocument& doc) override { return false; };

private:
  Hardware* items_[10]; // non-owning pointers
  unsigned int count_;  // number of active items
};

#endif // __GAVEL_HARDWARE_H