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

class EthernetModule : public Task, public VirtualNetwork, public VirtualServerFactory, public Hardware {
public:
  EthernetModule();
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override;
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;

  void configure();
  void allowDHCP(bool __allowDHCP);
  void configure(byte* __macAddress, bool __isDHCP);
  void configure(byte* __macAddress, bool __allowDHCP, bool __isDHCP, byte* __ipAddress, byte* __dnsAddress,
                 byte* __subnetMask, byte* __gatewayAddress);

  bool linkStatus() const;
  IMemory* getMemory() { return &memory; };
  IPAddress getIPAddress();
  IPAddress getSubnetMask();
  IPAddress getGateway();
  IPAddress getDNS();
  byte* getMACAddress() { return memory.memory.data.macAddress; };
  byte* getIPAddressByte() { return memory.memory.data.ipAddress; };
  byte* getSubnetMaskByte() { return memory.memory.data.subnetMask; };
  bool getDHCP() { return memory.memory.data.isDHCP; };
  VirtualServer* getServer(int port);
  virtual bool isWorking() const override { return (hardwareStatus && linkStatus()); };

private:
  bool resetW5500();
  bool setupW5500();
  bool updateMemory();
  void setAllowDHCP(bool allow) { memory.memory.data.allowDHCP = allow; };

  // Only used for initial configuration
  EthernetMemory memory;

  OutputInterface* terminal;

  void ipConfig(OutputInterface* terminal);
  void ifConfig(OutputInterface* terminal);
  bool hardwareStatus = false;
};

#endif //__GAVEL_ETHERNET_MODULE_H
