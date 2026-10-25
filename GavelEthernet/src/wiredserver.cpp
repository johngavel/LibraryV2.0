#include "wiredserver.h"

Client* WiredClientManager::setClient(EthernetClient __client) {
  for (unsigned int i = 0; i < MAX_CLIENTS; i++) {
    if (!client[i]) {
      client[i] = __client;
      return &client[i];
    }
  }
  __client.stop();
  return (&errorClient);
}

void WiredServer::begin() {
  server = new EthernetServer(port);
  server->begin();
}

Client* WiredServer::accept() {
  return clientManager.setClient(server->accept());
}
