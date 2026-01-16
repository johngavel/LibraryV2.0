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
  virtual void addCmd(TerminalCommand* __termCmd) override;
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override;
  virtual bool executeTask() override;

  void configure(VirtualServer* __server, DigitalFileSystem* __dfs) {
    server = __server;
    dfs = __dfs;
  };

  void setErrorPage(String name) { errorPage = name; };
  bool verifyPage(String name) { return dfs->verifyFile(name.c_str()); };
  VirtualServer* getServer() { return server; };
  void clientCmd(OutputInterface* terminal);

private:
  VirtualServer* server = nullptr;
  DigitalFileSystem* dfs = nullptr;
  String errorPage = "";
  ClientFilePool clientPool;
};

#endif // __GAVEL_SERVER_MODULE_H
