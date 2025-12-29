#ifndef __GAVEL_IP_INFO_FILE_H
#define __GAVEL_IP_INFO_FILE_H

#include <ArduinoJson.h>
#include <GavelDebug.h>
#include <GavelEthernet.h>
#include <GavelFileSystem.h>
#include <GavelUtil.h>

void convertFromJson(JsonVariantConst src, IPAddress& dst) {
  dst.fromString(src.as<const char*>());
}

class IpInfoFile : public StreamFile {
public:
  IpInfoFile(EthernetMemory* mem) : StreamFile("ip-info.json", READ_WRITE), _memory(mem){};

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
    clear();
    return write((const unsigned char*) json, strnlen(json, sizeof(json)));
  };

  virtual bool parseWriteData() override {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, *this);
    if (error) return false;
    bool isDHCP = doc["isDHCP"];
    const char* ipAddress = doc["ipAddress"];
    const char* subnetMask = doc["subnetMask"];
    const char* gatewayAddress = doc["gatewayAddress"];
    const char* dnsAddress = doc["dnsAddress"];

    if (_memory->memory.data.allowDHCP) {
      if (isDHCP)
        DEBUG("Setting DHCP");
      else
        DEBUG("Clearing DHCP");
      //_memory->memory.data.isDHCP = isDHCP;
    }

    if (ipAddress) { DEBUG(ipAddress); }
    if (subnetMask) { DEBUG(subnetMask); }
    if (gatewayAddress) { DEBUG(gatewayAddress); }
    if (dnsAddress) { DEBUG(dnsAddress); }

    return true;
  };

private:
  EthernetMemory* _memory;
};

#endif // __GAVEL_IP_INFO_FILE_H
