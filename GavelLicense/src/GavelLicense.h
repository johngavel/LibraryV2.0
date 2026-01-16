#ifndef __GAVEL_LICENSE_H
#define __GAVEL_LICENSE_H

#include "libraries.h"

#include <GavelTask.h>
#include <GavelUtil.h>
#include <asciitable/asciitable.h>

class License : public Task, public JsonInterface {
public:
  License() : Task("License"){};

  virtual void addCmd(TerminalCommand* __termCmd) override {
    if (__termCmd) {
      __termCmd->addCmd("lib", "", "Prints the list of libraries used by this program",
                        [this](TerminalLibrary::OutputInterface* terminal) { libraryCmd(terminal); });
      __termCmd->addCmd("library", "[n]", "Prints the details of a library used by this program",
                        [this](TerminalLibrary::OutputInterface* terminal) { detailCmd(terminal); });
    }
  };
  virtual void reservePins(BackendPinSetup* pinsetup) override {};
  virtual bool setupTask(OutputInterface* __terminal) override { return true; };
  virtual bool executeTask() override { return true; };

  bool addLibrary(unsigned int index) {
    if (index >= LIBRARY_COUNT) return false;
    return licenseList.push((void*) &libraries[index]);
  }

  bool addLibrary(const char* name, const char* version, const char* license_name, const char* link) {
    LibraryInfo info;
    info.name = name;
    info.version = version;
    info.license_name = license_name;
    info.link = link;
    return licenseList.push((void*) &info);
  }
  /* JsonInterface Overrides */
  virtual JsonDocument createJson() override {
    JsonDocument doc;
    doc["countLibrary"] = licenseList.count();
    // Make the top-level a JSON array and fill it from the C array
    JsonArray array = doc["library"].to<JsonArray>();
    for (unsigned long i = 0; i < licenseList.count(); i++) {
      LibraryInfo* info = (LibraryInfo*) licenseList.get(i);
      JsonObject object = array.add<JsonObject>();
      object["name"] = info->name;
      object["version"] = info->version;
      object["license_name"] = info->license_name;
      object["link"] = info->link;
    }
    return doc;
  };
  virtual bool parseJson(JsonDocument& doc) override { return false; };

private:
  ClassicQueue licenseList = ClassicQueue(MAX_LIBRARIES, sizeof(LibraryInfo));

  void libraryCmd(OutputInterface* terminal) {
    AsciiTable table(terminal);
    char numBuf[8];
    table.addColumn(Cyan, "#", 4);
    table.addColumn(Normal, "Name", 20);
    table.addColumn(Yellow, "Version", 9);
    for (unsigned long i = 0; i < licenseList.count(); i++) {
      numToA(i + 1, numBuf, sizeof(numBuf));
      LibraryInfo* info = (LibraryInfo*) licenseList.get(i);
      table.printData(numBuf, info->name, info->version);
    }
    table.printDone("All Libraries");
    terminal->prompt();
  };

  void detailCmd(OutputInterface* terminal) {
    char* value = terminal->readParameter();
    if (!value) {
      terminal->invalidParameter();
      terminal->println();
      terminal->prompt();
      return;
    }
    unsigned int index = (unsigned long) atoi(value) - 1;
    if (index >= licenseList.count()) {
      terminal->invalidParameter();
      terminal->println();
      terminal->prompt();
      return;
    }
    LibraryInfo* info = (LibraryInfo*) licenseList.get(index);
    terminal->println(HELP, "Name: ", info->name);
    terminal->println(HELP, "Version: ", info->version);
    terminal->println(HELP, "LicenseName: ", info->license_name);
    terminal->println(HELP, "Link: ", info->link);
    terminal->println();
    terminal->prompt();
  };
};

#endif // __GAVEL_LICENSE_H
