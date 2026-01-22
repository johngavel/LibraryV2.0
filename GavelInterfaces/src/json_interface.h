#ifndef __GAVEL_JSON_INTERFACE_H
#define __GAVEL_JSON_INTERFACE_H

#include <ArduinoJson.h>
#include <Terminal.h>

class JsonInterface {
public:
  virtual JsonDocument createJson() = 0;
  virtual bool parseJson(JsonDocument& doc) = 0;

  bool create(Stream& stream) {
    serializeJson(createJson(), stream);
    return true;
  };
  bool create(char* buffer, size_t length) {
    serializeJson(createJson(), buffer, length);
    return true;
  };
  bool parse(Stream& stream) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, stream);
    if (error) { return false; }
    return parseJson(doc);
  };
  bool parse(char* buffer, size_t length) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, buffer, length);
    if (error) { return false; }
    return parseJson(doc);
  };
};

#endif // __GAVEL_JSON_INTERFACE_H