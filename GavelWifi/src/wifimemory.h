#ifndef __GAVEL_WIFI_MEMORY_H
#define __GAVEL_WIFI_MEMORY_H

#include <Credential.h>

#ifndef STASSID
#define STASSID "none"
#endif

#ifndef STAPSK
#define STAPSK "none"
#endif

#include <GavelInterfaces.h>
#include <GavelUtil.h>

class WifiMemory : public IMemory {
public:
  typedef struct {
    char ssid[16];
    char password[16];
    unsigned char spare[16];
  } WifiData;

  static_assert(sizeof(WifiData) == 48, "ProgramMemory size unexpected - check packing/padding.");

  typedef union {
    WifiData data;
    unsigned char buffer[sizeof(WifiData)];
  } WifiUnion;

  WifiMemory() : IMemory("Wifi Config") { memset(memory.buffer, 0, sizeof(WifiUnion::buffer)); };

  // IMemory overrides
  virtual const unsigned char& operator[](std::size_t index) const override { return memory.buffer[index]; }
  virtual unsigned char& operator[](std::size_t index) override { return memory.buffer[index]; }
  void updateExternal() { setInternal(true); };

  virtual std::size_t size() const noexcept override { return sizeof(WifiUnion::buffer); }

  virtual void initMemory() override {
    memset(memory.data.ssid, 0, sizeof(memory.data.ssid));
    memset(memory.data.password, 0, sizeof(memory.data.password));
    memset(memory.data.spare, 0, sizeof(memory.data.spare));
    strncpy(memory.data.ssid, STASSID, sizeof(memory.data.ssid) - 1);
    strncpy(memory.data.password, STAPSK, sizeof(memory.data.password) - 1);
  }

  virtual void printData(OutputInterface* terminal) override {
    StringBuilder sb;
    sb = "Wifi Network: ";
    sb + memory.data.ssid;
    terminal->println(INFO, sb.c_str());
    sb = "Wifi Password: ";
    sb + "********";
    terminal->println(INFO, sb.c_str());
  }
  WifiUnion memory;

  virtual JsonDocument createJson() override {
    JsonDocument doc;
    doc["wifiNetwork"] = memory.data.ssid;
    doc["wifiPassword"] = "********";
    doc["isWifi"] = true;

    return doc;
  };

  virtual bool parseJson(JsonDocument& doc) override { return true; };

private:
};

#endif // __GAVEL_WIFI_MEMORY_H