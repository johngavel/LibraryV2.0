#include "servermodule.h"

#include "asciitable/asciitable.h"
#include "serverhelper.h"

#include <GavelSPIWire.h>

// Server Mdoule Methods
ServerModule::ServerModule() : Task("HTTPServer") {
  setRefreshMilli(10);
}

void ServerModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) {
    __termCmd->addCmd("client", "", "Prints a list of Tasks running in the system",
                      [this](TerminalLibrary::OutputInterface* terminal) { clientCmd(terminal); });
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

void ServerModule::clientCmd(OutputInterface* terminal) {
  AsciiTable table(terminal);

  bool verbose = false;
  char* parameter = terminal->readParameter();
  if ((parameter != NULL) && (safeCompare(parameter, "-v") == 0)) verbose = true;

  table.addColumn(Magenta, "ID", 6);
  table.addColumn(Green, "Used", 7);
  table.addColumn(Green, "Valid", 7);
  table.addColumn(Yellow, "State", 7);
  table.addColumn(Normal, "File", 20);
  table.addColumn(Normal, "File Stat", 7);
  table.printHeader();
  for (unsigned int i = 0; i < clientPool.capacity(); i++) {
    ClientFileEntry* cfe = clientPool.at(i);
    StringBuilder idString = i;
    StringBuilder usedString = cfe->used;
    StringBuilder validString = cfe->isValid();
    StringBuilder stateString = cfe->connection.state;
    StringBuilder fileString = (cfe->connection.file) ? cfe->connection.file->name() : "none";
    StringBuilder fileStatString = (cfe->connection.file->getPermission() == READ_ONLY)? "R ": "W ";
    if (cfe->connection.file)
      fileStatString += (cfe->connection.file->getMode() == READ_MODE)? "Read":"Write";
    else
      fileStatString += "Close";
    if (cfe->isValid() || verbose)
      table.printData(idString.c_str(), usedString.c_str(), validString.c_str(), stateString.c_str(),
                      fileString.c_str(), fileStatString.c_str());
  }
  table.printDone("Client Done");
  terminal->prompt();
}
