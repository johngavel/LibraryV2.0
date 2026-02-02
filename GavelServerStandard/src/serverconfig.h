#ifndef __GAVEL_SERVER_CONFIG_H
#define __GAVEL_SERVER_CONFIG_H

#include <GavelInterfaces.h>

class ServerConfig : public JsonInterface {
public:
  ServerConfig(){};
  virtual JsonDocument createJson() override {
    JsonDocument doc;
    doc["programInfo"] = programInfo;
    doc["ethernetInfo"] = ethernetInfo;
    doc["memoryInfo"] = memoryInfo;
    doc["licenseInfo"] = licenseInfo;
    doc["hwInfo"] = hwInfo;
    doc["rebootInfo"] = rebootInfo;
    doc["upgradeInfo"] = upgradeInfo;
    doc["uploadInfo"] = uploadInfo;
    doc["terminalInfo"] = terminalInfo;
    return doc;
  };
  virtual bool parseJson(JsonDocument& doc) override {
    if (!doc["programInfo"].isNull()) { programInfo = doc["programInfo"]; }
    if (!doc["ethernetInfo"].isNull()) { ethernetInfo = doc["ethernetInfo"]; }
    if (!doc["memoryInfo"].isNull()) { memoryInfo = doc["memoryInfo"]; }
    if (!doc["licenseInfo"].isNull()) { licenseInfo = doc["licenseInfo"]; }
    if (!doc["hwInfo"].isNull()) { hwInfo = doc["hwInfo"]; }
    if (!doc["rebootInfo"].isNull()) { rebootInfo = doc["rebootInfo"]; }
    if (!doc["upgradeInfo"].isNull()) { upgradeInfo = doc["upgradeInfo"]; }
    if (!doc["uploadInfo"].isNull()) { uploadInfo = doc["uploadInfo"]; }
    if (!doc["terminalInfo"].isNull()) { terminalInfo = doc["terminalInfo"]; }
    return false;
  };

  bool programInfo = false;
  bool ethernetInfo = false;
  bool memoryInfo = false;
  bool licenseInfo = false;
  bool hwInfo = false;
  bool rebootInfo = false;
  bool upgradeInfo = false;
  bool uploadInfo = false;
  bool terminalInfo = false;

private:
  char _fileBuffer[400];
  DeviceCmd* _device;
};

#endif // __GAVEL_SERVER_CONFIG_H
