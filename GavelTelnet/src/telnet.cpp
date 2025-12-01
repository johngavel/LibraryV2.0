#include "telnet.h"

#include <GavelSPIWire.h>

#define BUFFER_SIZE 1024
#define HEADER_LENGTH 4096

static char taskname[] = "Telnet";

TelnetModule::TelnetModule() : Task(taskname) {
  setRefreshMilli(10);
  client = nullptr;
  terminal_ = nullptr;
  clientConnected = false;
  server = nullptr;
  bannerFunction = nullptr;
  strncpy(promptString, "telnet:\\>", 20);
}

void TelnetModule::addCmd(TerminalCommand* __termCmd) {
  if (__termCmd) __termCmd->addCmd("exit", "", "Closes the Telnet Session.", [this](TerminalLibrary::OutputInterface* terminal) { closeTelnet(terminal); });
}

bool TelnetModule::setupTask(OutputInterface* __terminal) {
  spiWire.wireTake();
  server->begin();
  spiWire.wireGive();
  return true;
}

bool TelnetModule::executeTask() {
  if (!clientConnected) {
    spiWire.wireTake();
    client = server->accept();
    spiWire.wireGive();
    if (client->connected()) {
      clientConnected = true;
      if (terminal_ == nullptr) {
        terminal_ = new Terminal(client);
        terminal_->setup();
        terminal_->setEcho(true);
        terminal_->setColor(true);
        if (bannerFunction) terminal_->setBannerFunction(bannerFunction);
        terminal_->setPromptString(promptString);
        terminal_->setTerminalName("Telnet");
      } else {
        terminal_->setStream(client);
      }
      spiWire.wireTake();
      client->print("\x1B[?25h");
      client->print("\xFF\xFB\x01");
      client->println("Starting Telnet Session.");
      spiWire.wireGive();
      terminal_->banner();
      terminal_->prompt();
      spiWire.wireTake();
      client->flush();
      spiWire.wireGive();
    }
  } else {
    if (client->connected()) {
      terminal_->loop();
      spiWire.wireTake();
      client->flush();
      spiWire.wireGive();
    } else {
      clientConnected = false;
    }
  }
  return true;
}

void TelnetModule::closeTelnet(OutputInterface* terminal) {
  if (terminal == terminal_) {
    terminal->println(INFO, "Closing Telnet Session.");
    client->flush();
    client->stop();
  } else {
    terminal->println(ERROR, "Not supported on this terminal.");
    terminal->prompt();
  }
}
