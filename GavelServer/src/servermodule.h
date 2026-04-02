#ifndef __GAVEL_SERVER_MODULE_H
#define __GAVEL_SERVER_MODULE_H

#define MAX_PAGES 64
#define PAGE_NAME_LENGTH 64

#include "apifile.h"
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

  // --- NEW MONITORING METHODS ---
  void poolStatusCmd(OutputInterface* terminal);
  void getPoolStatistics(size_t& total, size_t& used, size_t& active, float& utilization);

private:
  VirtualServer* server = nullptr;
  DigitalFileSystem* dfs = nullptr;
  String errorPage = "";
  ClientFilePool clientPool;

  // --- NEW MONITORING VARIABLES ---
  unsigned long lastPoolWarning = 0;
  unsigned long poolWarningInterval = 30000; // 30 seconds between warnings
  float poolWarningThreshold = 80.0f;        // Warn when pool is 80% full
};

#endif // __GAVEL_SERVER_MODULE_H
