#ifndef __GAVEL_SERVER_MODULE_H
#define __GAVEL_SERVER_MODULE_H

#define MAX_PAGES 64
#define PAGE_NAME_LENGTH 64

#include <GavelInterfaces.h>
#include <GavelTask.h>

#define SERVER_DIRECTORY "/www"
#define API_DIRECTORY "/api"

class ServerModule : public Task {
public:
  ServerModule();
  void addCmd(TerminalCommand* __termCmd) override {};
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;

  void configure(VirtualServer* __server, DigitalFileSystem* __dfs) {
    server = __server;
    dfs = __dfs;
  };

  void setErrorPage(String name) { errorPage = name; };
  void setFavicon(const unsigned char* __favicon, unsigned int __faviconLength);
  bool verifyPage(String name) { return dfs->verifyFile(name.c_str()); };
  VirtualServer* getServer() { return server; };

private:
  bool serveFile(Client* client, const String& path);
  bool serveApi(Client* client, const String& path);
  VirtualServer* server = nullptr;
  DigitalFileSystem* dfs = nullptr;
  String errorPage = "";
};

#endif // __GAVEL_SERVER_MODULE_H
