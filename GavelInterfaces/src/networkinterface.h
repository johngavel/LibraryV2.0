#ifndef __GAVEL_NETWORK_INTERFACES_H
#define __GAVEL_NETWORK_INTERFACES_H

#include <Arduino.h>
#include <Client.h>

#define TELNET_PORT 23
#define HTTP_PORT 80
#define TEST_PORT 4242
#define MAX_CLIENTS 32

class VirtualNetwork {
public:
  virtual IPAddress getIPAddress() = 0;
  virtual IPAddress getSubnetMask() = 0;
  virtual IPAddress getGateway() = 0;
  virtual IPAddress getDNS() = 0;
  virtual bool getDHCP() = 0;
  virtual byte* getMACAddress() = 0;
};

class VirtualServer {
public:
  virtual void begin() = 0;
  virtual Client* accept() = 0;
  virtual VirtualNetwork* getNetworkInterface() = 0;
};

class VirtualServerFactory {
public:
  virtual VirtualServer* getServer(int port) = 0;
};

#endif // __GAVEL_NETWORK_INTERFACES_H