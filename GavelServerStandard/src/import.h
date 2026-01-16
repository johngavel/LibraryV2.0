#ifndef __GAVEL_IMPORT_FILE_H
#define __GAVEL_IMPORT_FILE_H

#include <ArduinoJson.h>
#include <GavelEEProm.h>
#include <GavelInterfaces.h>

/**
 * Merge all key/value pairs from src into dst.
 * - Later values overwrite earlier ones (simple policy).
 * - Null values in src are ignored.
 * - Handles nested objects recursively.
 * - Arrays are replaced (not appended) for simplicity.
 */
inline void mergeJsonObjects(JsonObject dst, JsonObjectConst src) {
  for (JsonPairConst kvp : src) {
    JsonVariantConst val = kvp.value();
    if (val.isNull()) continue;

    if (val.is<JsonObject>()) {
      // Merge nested object recursively
      JsonObject subDst = dst[kvp.key().c_str()].to<JsonObject>();
      mergeJsonObjects(subDst, val.as<JsonObjectConst>());
    } else {
      // Scalars and arrays: overwrite
      dst[kvp.key().c_str()] = val;
    }
  }
}

class ImportFile : public JsonInterface {
public:
  ImportFile(EEpromMemory* eeprom) : _eeprom(eeprom) {}

  virtual JsonDocument createJson() override {
    JsonDocument doc;
    if (!_eeprom) { return doc; }

    JsonObject merged = doc.to<JsonObject>();

    const unsigned long n = _eeprom->getNumberOfData();
    for (unsigned long i = 0; i < n; ++i) {
      auto* item = _eeprom->getData(i);
      if (!item) continue;

      JsonDocument part = item->createJson();
      if (!part.is<JsonObject>()) continue;

      mergeJsonObjects(merged, part.as<JsonObjectConst>());
    }

    return doc;
  }

  virtual bool parseJson(JsonDocument& doc) override {
    if (!_eeprom) return false;
    const unsigned long n = _eeprom->getNumberOfData();
    for (unsigned long i = 0; i < n; ++i) {
      auto* item = _eeprom->getData(i);
      if (!item) continue;
      item->parseJson(doc);
    }
    return true;
  }

private:
  EEpromMemory* _eeprom;
};

#endif // __GAVEL_IMPORT_FILE_H
