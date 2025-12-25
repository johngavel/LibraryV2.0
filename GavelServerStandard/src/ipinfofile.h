#ifndef __GAVEL_IP_INFO_FILE_H
#define __GAVEL_IP_INFO_FILE_H

#include <ArduinoJson.h>
#include <GavelEthernet.h>
#include <GavelFileSystem.h>
#include <GavelUtil.h>

class IpInfoFile : public DynamicFile {
public:
  IpInfoFile(EthernetMemory* mem)
      : DynamicFile("ip-info.json", READ_ONLY, _fileBuffer, sizeof(_fileBuffer)), _memory(mem){};
  bool createReadData() override {
    char buffer[128];
    JsonDocument doc;
    char json[512];
    doc["macAddress"] = getMacString(_memory->memory.data.macAddress, buffer, sizeof(buffer));
    doc["ipAddress"] = getIPString(_memory->memory.data.ipAddress, buffer, sizeof(buffer));
    doc["subnetMask"] = getIPString(_memory->memory.data.subnetMask, buffer, sizeof(buffer));
    doc["gatewayAddress"] = getIPString(_memory->memory.data.gatewayAddress, buffer, sizeof(buffer));
    doc["dnsAddress"] = getIPString(_memory->memory.data.dnsAddress, buffer, sizeof(buffer));
    doc["isDHCP"] = _memory->memory.data.isDHCP;
    doc["allowDHCP"] = _memory->memory.data.allowDHCP;
    serializeJson(doc, json);
    return loadReadBuffer(json, strnlen(json, sizeof(json)));
  };

  virtual bool parseWriteData() override { return true; };

private:
  char _fileBuffer[512];
  EthernetMemory* _memory;
};

#endif // __GAVEL_IP_INFO_FILE_H
