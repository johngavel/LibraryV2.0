#ifndef __GAVEL_SERVER_MODULE_H
#define __GAVEL_SERVER_MODULE_H

#define MAX_PAGES 64
#define PAGE_NAME_LENGTH 64

#include "clientmanagment.h"
#include "serverhelper.h"

#include <GavelInterfaces.h>
#include <GavelTask.h>

class ServerModule : public Task {
public:
  ServerModule();
  void addCmd(TerminalCommand* __termCmd) override;
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  void configure(VirtualServer* __server, DigitalFileSystem* __dfs) {
    server = __server;
    dfs = __dfs;
  };

  void setErrorPage(String name) { errorPage = name; };
  bool verifyPage(String name) { return dfs->verifyFile(name.c_str()); };
  VirtualServer* getServer() { return server; };

private:
  VirtualServer* server = nullptr;
  DigitalFileSystem* dfs = nullptr;
  String errorPage = "";
  ClientFilePool clientPool;
};

#endif // __GAVEL_SERVER_MODULE_H
