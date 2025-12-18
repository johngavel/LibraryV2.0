#include "servermodule.h"

#include "serverhelper.h"

#include <GavelSPIWire.h>

// Server Mdoule Methods
ServerModule::ServerModule() : Task("HTTPServer") {
  setRefreshMilli(10);
}

void ServerModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) {
    // No Commands at this time.
  }
}

bool ServerModule::setupTask(OutputInterface* __terminal) {
  if (server) {
    spiWire.wireTake();
    server->begin();
    spiWire.wireGive();
  }
  return true;
}

bool ServerModule::executeTask() {
  Client* client;

  if (server) {
    spiWire.wireTake();
    client = server->accept();
    spiWire.wireGive();
    if (clientConnected(client)) clientPool.add(client, dfs, errorPage);

    for (unsigned int i = 0; i < clientPool.capacity(); i++) {
      ClientFileEntry* cfe = clientPool.at(i);
      if (cfe->used) cfe->connection.execute();
    }
    clientPool.sweepDisconnected();
  }
  return true;
}
