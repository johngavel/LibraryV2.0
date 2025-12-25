#ifndef __GAVEL_ETHERNET_MEMORY_H
#define __GAVEL_ETHERNET_MEMORY_H

#include <GavelInterfaces.h>
#include <GavelUtil.h>

class EthernetMemory : public IMemory {
public:
  typedef struct {
    unsigned char macAddress[6];
    unsigned char ipAddress[4];
    unsigned char subnetMask[4];
    unsigned char gatewayAddress[4];
    unsigned char dnsAddress[4];
    bool isDHCP;
    bool allowDHCP;
    unsigned char spare[8];
  } EthernetData;

  static_assert(sizeof(EthernetData) == 32, "ProgramMemory size unexpected - check packing/padding.");

  typedef union {
    EthernetData data;
    unsigned char buffer[sizeof(EthernetData)];
  } EthernetUnion;

  EthernetMemory() { memset(memory.buffer, 0, sizeof(EthernetUnion::buffer)); };

  // IMemory overrides
  const unsigned char& operator[](std::size_t index) const override { return memory.buffer[index]; }
  unsigned char& operator[](std::size_t index) override { return memory.buffer[index]; }
  void updateExternal() { setInternal(true); };

  std::size_t size() const noexcept override { return sizeof(EthernetUnion::buffer); }

  void initMemory() override {
    randomSeed(rp2040.hwrand32());
    memory.data.isDHCP = true;
    memory.data.allowDHCP = true;
    memory.data.macAddress[0] = 0xDE;
    memory.data.macAddress[1] = 0xAD;
    memory.data.macAddress[2] = 0xCC;
    memory.data.macAddress[3] = random(256);
    memory.data.macAddress[4] = random(256);
    memory.data.macAddress[5] = random(256);
    memory.data.ipAddress[0] = 0;
    memory.data.ipAddress[1] = 0;
    memory.data.ipAddress[2] = 0;
    memory.data.ipAddress[3] = 0;
    memory.data.dnsAddress[0] = 255;
    memory.data.dnsAddress[1] = 255;
    memory.data.dnsAddress[2] = 255;
    memory.data.dnsAddress[3] = 255;
    memory.data.subnetMask[0] = 255;
    memory.data.subnetMask[1] = 255;
    memory.data.subnetMask[2] = 255;
    memory.data.subnetMask[3] = 0;
    memory.data.gatewayAddress[0] = 255;
    memory.data.gatewayAddress[1] = 255;
    memory.data.gatewayAddress[2] = 255;
    memory.data.gatewayAddress[3] = 255;
    memset(memory.data.spare, 0, sizeof(memory.data.spare));
  }

  void printData(OutputInterface* terminal) override {
    StringBuilder sb;
    char buffer[20];
    terminal->println(HELP, "Ethernet Module Data: ");
    sb + "DHCP: " + memory.data.isDHCP;
    terminal->println(INFO, sb.c_str());
    sb = "MAC Address: ";
    sb + getMacString(memory.data.macAddress, buffer, sizeof(buffer));
    terminal->println(INFO, sb.c_str());
    sb = "IP Address: ";
    sb + getIPString(memory.data.ipAddress, buffer, sizeof(buffer));
    terminal->println(INFO, sb.c_str());
    sb = "Subnet Mask: ";
    sb + getIPString(memory.data.subnetMask, buffer, sizeof(buffer));
    terminal->println(INFO, sb.c_str());
    sb = "Gateway Address: ";
    sb + getIPString(memory.data.gatewayAddress, buffer, sizeof(buffer));
    terminal->println(INFO, sb.c_str());
    sb = "DNS Address: ";
    sb + getIPString(memory.data.dnsAddress, buffer, sizeof(buffer));
    terminal->println(INFO, sb.c_str());
  }
  EthernetUnion memory;

private:
};

#endif // __GAVEL_ETHERNET_MEMORY_H