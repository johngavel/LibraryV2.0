#ifndef __GAVEL_HARDWARE_FILE_H
#define __GAVEL_HARDWARE_FILE_H

#include <GavelInterfaces.h>
#include <GavelPicoStandard.h>

class HardwareFile : public JsonInterface {
public:
  HardwareFile(HardwareList* __hwList) { hwList = __hwList; };
  virtual JsonDocument createJson() override {
    JsonDocument doc;
    if (hwList) {
      doc["numberhw"] = hwList->size();
      JsonArray data = doc["hwtable"].to<JsonArray>();
      for (unsigned int i = 0; i < hwList->size(); i++) {
        JsonObject object = data.add<JsonObject>();
        object["hwname"] = (*hwList)[i]->getName();
        object["hwid"] = (*hwList)[i]->getId();
        object["hwstatus"] = (*hwList)[i]->isWorking();
      }
    }
    return doc;
  };

  virtual bool parseJson(JsonDocument& doc) override { return false; };

private:
  HardwareList* hwList = nullptr;
};

#endif // __GAVEL_HARDWARE_FILE_H