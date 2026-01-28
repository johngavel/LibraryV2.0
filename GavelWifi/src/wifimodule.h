#ifndef __GAVEL_WIFI
#define __GAVEL_WIFI

#include "wifimemory.h"

#include <Arduino.h>
#include <GavelInterfaces.h>
#include <GavelTask.h>
#include <WiFi.h>

class WirelessClientManager {
public:
  WirelessClientManager(){};
  Client* setClient(WiFiClient __client) {
    for (unsigned int i = 0; i < MAX_CLIENTS; i++) {
      if (!client[i].connected()) {
        client[i] = __client;
        return &client[i];
      }
    }
    return (&errorClient);
  }

private:
  WiFiClient client[MAX_CLIENTS];
  WiFiClient errorClient;
};

class WifiModule : public Task,
                   public VirtualNetwork,
                   public VirtualServerFactory,
                   public JsonInterface,
                   public Hardware {
public:
  WifiModule() : Task("WifiModule"), Hardware("WifiModule"){};
  bool linkStatus();
  String getSSID();
  WifiMemory* getMemory() { return &memory; };

  // Task Overrides
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;

  // Virtual Network Overrides
  virtual IPAddress getIPAddress() override;
  virtual IPAddress getSubnetMask() override;
  virtual IPAddress getGateway() override;
  virtual IPAddress getDNS() override;
  virtual byte* getMACAddress() override { return WiFi.macAddress(macAddress); };
  virtual bool getDHCP() override { return false; };

  // VirtualServerFactory Override
  virtual VirtualServer* getServer(int port) override;

  // JsonInterface Override
  virtual JsonDocument createJson() override;
  virtual bool parseJson(JsonDocument& doc) override;

  // Hardware Override
  virtual bool isWorking() const override { return status; };

private:
  bool setupWifi();

  WifiMemory memory;
  bool status = false;
  unsigned char macAddress[6];

  void wifiStat(OutputInterface* terminal);
  void wifiScan(OutputInterface* terminal);
};

class WifiVirtualServer : public VirtualServer {
public:
  WifiVirtualServer(VirtualNetwork* __network, int __port) : server(nullptr), network(__network), port(__port){};
  void begin();
  Client* accept();
  VirtualNetwork* getNetworkInterface() { return network; };

private:
  WiFiServer* server = nullptr;
  VirtualNetwork* network;
  WirelessClientManager clientManager;
  int port;
};

#endif
