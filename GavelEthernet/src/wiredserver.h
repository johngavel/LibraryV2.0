#ifndef __GAVEL_WIRE_SERVER_H
#define __GAVEL_WIRE_SERVER_H

#include <Ethernet.h>
#include <GavelInterfaces.h>

class WiredClientManager {
public:
  WiredClientManager(){};
  Client* setClient(EthernetClient __client);

private:
  EthernetClient client[MAX_CLIENTS];
  EthernetClient errorClient;
};

class WiredServer : public VirtualServer {
public:
  WiredServer(VirtualNetwork* __network, int __port) : network(__network), port(__port){};
  void begin();
  Client* accept();
  VirtualNetwork* getNetworkInterface() { return network; };

private:
  EthernetServer* server = nullptr;
  VirtualNetwork* network = nullptr;
  WiredClientManager clientManager;
  int port;
};

#endif // __GAVEL_WIRE_SERVER_H