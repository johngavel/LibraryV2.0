#ifndef __GAVEL_ETHERNET_MODULE_H
#define __GAVEL_ETHERNET_MODULE_H

#include "wiredserver.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <GavelInterfaces.h>
#include <GavelTask.h>
#include <SPI.h>
#include <Terminal.h>

class EthernetModule : public Task, public VirtualNetwork, public VirtualServerFactory {
public:
  EthernetModule();
  virtual void addCmd(TerminalCommand* __termCmd);
  virtual bool setupTask(OutputInterface* __terminal);
  virtual bool executeTask();

  void configure(byte* __macAddress, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress, byte* __subnetMask, byte* __gatewayAddress);
  void configure(byte* __macAddress, bool __isDHCP) { configure(__macAddress, __isDHCP, nullptr, nullptr, nullptr, nullptr); };
  bool linkStatus();
  IPAddress getIPAddress();
  IPAddress getSubnetMask();
  IPAddress getGateway();
  IPAddress getDNS();
  byte* getMACAddress() { return macAddress; };
  bool getDHCP() { return isDHCP; };
  VirtualServer* getServer(int port);
  bool ipChanged = false;
  // Only used for initial configuration
  bool isConfigured = false;
  byte* macAddress = nullptr;
  bool isDHCP = false;

private:
  bool resetW5500();
  bool setupW5500();

  // Only used for initial configuration
  byte* ipAddress = nullptr;
  byte* dnsAddress = nullptr;
  byte* subnetMask = nullptr;
  byte* gatewayAddress = nullptr;

  OutputInterface* terminal;

  void ipStat(OutputInterface* terminal);
};

#endif //__GAVEL_ETHERNET_MODULE_H
