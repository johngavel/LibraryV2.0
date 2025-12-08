#ifndef __GAVEL_TELNET_CLASS_H
#define __GAVEL_TELNET_CLASS_H

#include "networkinterface.h"

#include <GavelTask.h>
#include <Terminal.h>

class TelnetModule : public Task {
public:
  TelnetModule();
  void configure(VirtualServer* __server, const char* __promptString, void (*function)(OutputInterface*)) {
    server = __server;
    strncpy(promptString, __promptString, 20);
    bannerFunction = function;
  };
  void addCmd(TerminalCommand* __termCmd) override;
  void reservePins(BackendPinSetup* pinsetup) override {};
  bool setupTask(OutputInterface* __terminal) override;
  bool executeTask() override;
  VirtualServer* getServer() { return server; };
  Terminal* terminal_;
  Client* client;

private:
  bool clientConnected;
  VirtualServer* server;
  void closeTelnet(OutputInterface* terminal);
  void (*bannerFunction)(OutputInterface*) = nullptr;
  char promptString[20];
};

#endif // __GAVEL_TELNET_CLASS_H
