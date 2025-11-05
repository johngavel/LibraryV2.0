#ifndef __GAVEL_ETHERNET_MODULE_H
#define __GAVEL_ETHERNET_MODULE_H

#include "ethernetmemory.h"
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

  void configure();
  void configure(byte* __macAddress, bool __isDHCP);
  void configure(byte* __macAddress, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress, byte* __subnetMask, byte* __gatewayAddress);

  bool linkStatus();
  IMemory* getMemory() { return &memory; };
  IPAddress getIPAddress();
  IPAddress getSubnetMask();
  IPAddress getGateway();
  IPAddress getDNS();
  byte* getMACAddress() { return memory.memory.data.macAddress; };
  bool getDHCP() { return memory.memory.data.isDHCP; };
  VirtualServer* getServer(int port);

private:
  bool resetW5500();
  bool setupW5500();
  bool updateMemory();

  // Only used for initial configuration
  EthernetMemory memory;

  OutputInterface* terminal;

  void ipStat(OutputInterface* terminal);
};

#endif //__GAVEL_ETHERNET_MODULE_H
