#ifndef __GAVEL_DEBUG_API_H
#define __GAVEL_DEBUG_API_H

#include "api.h"

#include <GavelServer.h>

class DebugAPI : public API {
public:
  DebugAPI() : API(DEFAULT_BUFFER_SIZE){};
  DebugAPI(unsigned int size) : API(size){};

  void debugPrint() {
    DBG_PRINTF("Debug API Method: %s\r\n", method_.get());
    DBG_PRINTF("Query String Keys: %d\r\n", query_.size());
    for (unsigned int i = 0; i < query_.size(); i++) {
      DBG_PRINTF("  Key: %s Value: %s \r\n", query_.keyAt(i), query_.valueAt(i));
    }
    DBG_PRINTF("Meta Headers: %d\r\n", metaHeaders_.size());
    for (unsigned int i = 0; i < metaHeaders_.size(); i++) {
      DBG_PRINTF("  Key: %s Value: %s \r\n", metaHeaders_.keyAt(i), metaHeaders_.valueAt(i));
    }
    DBG_PRINTLN();
  }

  virtual JsonDocument createJson() override {
    JsonDocument doc;
    DBG_PRINTLNS("Creating JSON for DEBUG API.");
    debugPrint();
    doc["debug"] = true;
    return doc;
  };
  virtual bool parseJson(JsonDocument& doc) override {
    DBG_PRINTLNS("Parsing JSON for DEBUG API.");
    debugPrint();
    return true;
  };
};

#endif