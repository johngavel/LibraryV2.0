#ifndef __GAVEL_IP_INFO_FILE_H
#define __GAVEL_IP_INFO_FILE_H

#include <GavelEthernet.h>
#include <GavelFileSystem.h>
#include <GavelUtil.h>

class IpInfoFile : public DynamicFile {
public:
  IpInfoFile(EthernetMemory* mem)
      : DynamicFile("ip-info.json", READ_ONLY, _fileBuffer, sizeof(_fileBuffer)), _memory(mem){};
  bool createReadData() override {
    char buffer[20];
    TinyJsonBuilder jb;
    jb.beginObject();
    jb.add("macAddress", getMacString(_memory->memory.data.macAddress, buffer, sizeof(buffer)));
    jb.add("ipAddress", getIPString(_memory->memory.data.ipAddress, buffer, sizeof(buffer)));
    jb.add("subnetMask", getIPString(_memory->memory.data.subnetMask, buffer, sizeof(buffer)));

    jb.add("gatewayAddress", getIPString(_memory->memory.data.gatewayAddress, buffer, sizeof(buffer))); // numeric
    jb.add("dnsAddress", getIPString(_memory->memory.data.dnsAddress, buffer, sizeof(buffer)));

    jb.add("isDHCP", (_memory->memory.data.isDHCP) ? "true" : "false");
    jb.endObject();

    return loadReadBuffer(jb.str().c_str(), jb.str().length()); // finalize JSON
  };

  virtual bool parseWriteData() override { return true; };

private:
  char _fileBuffer[200];
  EthernetMemory* _memory;
};

#endif // __GAVEL_IP_INFO_FILE_H
