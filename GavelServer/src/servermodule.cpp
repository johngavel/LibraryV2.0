#include "servermodule.h"

#include "asciitable/asciitable.h"
#include "serverdebug.h"
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

    // Add new pool status command
    __termCmd->addCmd("poolstatus", "", "Shows client pool usage statistics",
                      [this](TerminalLibrary::OutputInterface* terminal) { poolStatusCmd(terminal); });
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

    // Monitor pool usage when new client connects
    if (clientConnected(client)) {
      size_t total, used, active, stale;
      clientPool.getPoolStatus(total, used, active, stale);

#ifdef DEBUG_SERVER
      DBG_PRINTF("New Client - Pool: %d/%d used (%d active, %d stale) %.1f%%\r\n", used, total, active, stale,
                 clientPool.utilizationPercent());
#endif

      // Check for pool exhaustion
      if (!clientPool.hasSpace()) {
#ifdef DEBUG_SERVER
        DBG_PRINTF("Client pool full! Forcing cleanup of %d stale connections\r\n", stale);
#endif
        clientPool.forceCleanup();

        // If still no space after cleanup, reject connection
        if (!clientPool.hasSpace()) {
#ifdef DEBUG_SERVER
          DBG_PRINTF("Pool still full after cleanup, rejecting connection\r\n");
#endif
          client->stop();
          client = nullptr;
        }
      }

      // Warn if pool utilization is high
      float utilization = clientPool.utilizationPercent();
      if (utilization > poolWarningThreshold && (millis() - lastPoolWarning) > poolWarningInterval) {
#ifdef DEBUG_SERVER
        DBG_PRINTF("WARNING: Client pool utilization high: %.1f%%\r\n", utilization);
#endif
        lastPoolWarning = millis();
      }

      if (client) { clientPool.add(client, dfs, errorPage); }
    }

    // Execute client connections with monitoring
    for (unsigned int i = 0; i < clientPool.capacity(); i++) {
      ClientFileEntry* cfe = clientPool.at(i);
      if (cfe->used) { cfe->connection.execute(); }
    }

#ifdef DEBUG_SERVER
    if (executionErrors > 0) { DBG_PRINTF("Client execution errors: %d\r\n", executionErrors); }
#endif

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
  table.addColumn(Normal, "File", 24);
  table.addColumn(Normal, "File Stat", 10);
  table.printHeader();
  for (unsigned int i = 0; i < clientPool.capacity(); i++) {
    ClientFileEntry* cfe = clientPool.at(i);
    StringBuilder idString = i;
    StringBuilder usedString = cfe->used;
    StringBuilder validString = cfe->isValid();
    StringBuilder stateString = cfe->connection.state;
    StringBuilder fileString = (cfe->connection.file) ? cfe->connection.file->name() : "none";

    // Fix: Check if file exists before accessing its methods
    StringBuilder fileStatString;
    if (cfe->connection.file) {
      fileStatString = (cfe->connection.file->getPermission() == READ_ONLY) ? "R " : "W ";
      fileStatString += (cfe->connection.file->getMode() == READ_MODE) ? "Read" : "Write";
    } else {
      fileStatString = "Close";
    }

    if (cfe->isValid() || verbose)
      table.printData(idString.c_str(), usedString.c_str(), validString.c_str(), stateString.c_str(),
                      fileString.c_str(), fileStatString.c_str());
  }
  table.printDone("Client Done");
  terminal->prompt();
}
// New method to show pool status
void ServerModule::poolStatusCmd(OutputInterface* terminal) {
  size_t total, used, active, stale;
  clientPool.getPoolStatus(total, used, active, stale);
  float utilization = clientPool.utilizationPercent();

  terminal->println(INFO, "=== Client Pool Status ===");

  StringBuilder totalStr = "Total Slots: ";
  totalStr += total;
  terminal->println(INFO, totalStr.c_str());

  StringBuilder usedStr = "Used Slots: ";
  usedStr += used;
  terminal->println(INFO, usedStr.c_str());

  StringBuilder activeStr = "Active Connections: ";
  activeStr += active;
  terminal->println(INFO, activeStr.c_str());

  StringBuilder staleStr = "Stale Connections: ";
  staleStr += stale;
  terminal->println(INFO, staleStr.c_str());

  StringBuilder utilizationStr = "Utilization: ";
  utilizationStr += utilization;
  utilizationStr += "%";
  terminal->println(INFO, utilizationStr.c_str());

  StringBuilder availableStr = "Available Slots: ";
  availableStr += (total - used);
  terminal->println(INFO, availableStr.c_str());

  if (utilization > 90.0f) {
    terminal->println(ERROR, "WARNING: Pool utilization critical!");
  } else if (utilization > 75.0f) {
    terminal->println(WARNING, "CAUTION: Pool utilization high");
  }

  terminal->prompt();
}
// Helper method to get statistics programmatically
void ServerModule::getPoolStatistics(size_t& total, size_t& used, size_t& active, float& utilization) {
  size_t stale;
  clientPool.getPoolStatus(total, used, active, stale);
  utilization = clientPool.utilizationPercent();
}
